#include <Windows.h>
#include <iostream>
#include <string>
#include <chrono>
#include "Game.h"
#include "AStar.h"
#include "args.hxx"

constexpr int TARGET_FPS = 60;
constexpr double DESIRED_FRAME_TIME = 1000.0 / TARGET_FPS;

constexpr int GAME_AIR = 0;
constexpr int GAME_WALL = 1;
constexpr int GAME_START = 2;
constexpr int GAME_FINISH = 3;

LARGE_INTEGER lastFrameTime;
Game game = Game(Grid(0, 0, 0, 0));

COLORREF airColor = RGB(67, 65, 65); // gray
COLORREF wallColor = RGB(255, 0, 0); 
COLORREF startColor = RGB(44, 27, 107); // dark blue
COLORREF finishColor = RGB(108, 71, 247); // light blue
COLORREF wayColor = RGB(71, 247, 75); // lime green

bool ArePointsNotEqual(Point a, Point b)
{
    return (a.x != b.x || a.y != b.y);
}

double GetElapsedMilliseconds()
{
    LARGE_INTEGER currentTime;
    LARGE_INTEGER frequency;
    QueryPerformanceCounter(&currentTime);
    QueryPerformanceFrequency(&frequency);
    return static_cast<double>(currentTime.QuadPart - lastFrameTime.QuadPart) * 1000.0 / static_cast<double>(frequency.QuadPart);
}

void DrawRectangle(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    HBRUSH hBrush = CreateSolidBrush(color);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    RECT rect;
    rect.left = x1;
    rect.top = y1;
    rect.right = x2;
    rect.bottom = y2;

    FillRect(hdc, &rect, hBrush);

    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}


void DrawGrid(HDC hdc)
{

    for (int row = 0; row < game.grid.rows; ++row)
    {
        for (int col = 0; col < game.grid.cols; ++col)
        {
            int x1 = col * game.grid.size + col * game.grid.spacing;
            int y1 = row * game.grid.size + row * game.grid.spacing;
            int x2 = x1 + game.grid.size;
            int y2 = y1 + game.grid.size;

            int squareType = game.grid.getCell(row, col);;
            switch (squareType) {

            case (GAME_AIR): 
                DrawRectangle(hdc, x1, y1, x2, y2, airColor);
                break;
            
            case (GAME_WALL):
                DrawRectangle(hdc, x1, y1, x2, y2, wallColor);
                break;
            
            case (GAME_START): 
                DrawRectangle(hdc, x1, y1, x2, y2, startColor);
                break;

            case (GAME_FINISH):
                DrawRectangle(hdc, x1, y1, x2, y2, finishColor);
                break;
            
            }
        }
    }

}

void DrawWay(HDC hdc) {
    std::vector<Point> path = game.grid.findPath();
    for (Point p : path) {
        if (
            ArePointsNotEqual(p, game.grid.start)
            && ArePointsNotEqual(p, game.grid.finish)
            ) 
        {
            // should be p.x * (...) and p.y * (...) but idk something is switched and it works that way
            int x1 = p.y * (game.grid.size + game.grid.spacing);
            int y1 = p.x * (game.grid.size + game.grid.spacing);

            int x2 = x1 + game.grid.size;
            int y2 = y1 + game.grid.size;

            DrawRectangle(hdc, x1, y1, x2, y2, wayColor);
        }
    }

    std::wstring posString = L"Way length: " + std::to_wstring(path.size());
    TextOut(hdc, 10, 10, posString.c_str(), posString.length());
}

bool GetMouseSquare(Point* point)
{
    Point square;
    square.x = game.mouse.pos.x / (game.grid.size + game.grid.spacing);
    square.y = game.mouse.pos.y / (game.grid.size + game.grid.spacing);

    float offsetX = game.mouse.pos.x % (game.grid.size + game.grid.spacing);
    float offsetY = game.mouse.pos.y % (game.grid.size + game.grid.spacing);

    if (offsetX >= game.grid.size || offsetY >= game.grid.size) {
        // Mouse is over the spacing, not a valid square
        return false;
    }

    if (0 <= square.x && square.x < game.grid.cols && 0 <= square.y && square.y < game.grid.rows) {
        *point = square;
        return true;
    }

    return false;
}


void GameUpdate()
{
    Point curSquare;
    if (
        game.mouse.lButtonDown
        && GetMouseSquare(&curSquare)
        && game.grid.getCell(curSquare.y, curSquare.x) != GAME_WALL
        && ArePointsNotEqual(curSquare, game.grid.start)
        && ArePointsNotEqual(curSquare, game.grid.finish)
        ) {
        game.grid.setCell(curSquare.y, curSquare.x, GAME_WALL);
        game.grid.changed = true;
    }

    if (
        game.mouse.rButtonDown
        && GetMouseSquare(&curSquare)
        && game.grid.getCell(curSquare.y, curSquare.x) != GAME_AIR
        && ArePointsNotEqual(curSquare, game.grid.start)
        && ArePointsNotEqual(curSquare, game.grid.finish)
        ) {
        game.grid.setCell(curSquare.y, curSquare.x, GAME_AIR);
        game.grid.changed = true;
    }
}

void GameRender(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);


    if (game.grid.changed) {
        DrawGrid(hdc);
        DrawWay(hdc);
    }
    game.grid.changed = false;

    /*std::wstring posString = L"Mouse position: (" + std::to_wstring(game.mouse.pos.x) + L", " + std::to_wstring(game.mouse.pos.y) + L")";
    TextOut(hdc, 10, 10, posString.c_str(), posString.length());*/

    EndPaint(hwnd, &ps);
    ReleaseDC(hwnd, hdc);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }

    case WM_SETCURSOR: {
        if (LOWORD(lParam) == HTCLIENT)
        {
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return TRUE;
        }
    }
    
    // replaced with GetAsyncKeyState in WinMain
    /* case WM_LBUTTONDOWN: {
        std::cout << "lbtn down" << std::endl;
        game.mouse.lButtonDown = true;
    }*/   

    // replaced with GetAsyncKeyState in WinMain
    /* case WM_LBUTTONUP: {
        std::cout << "lbtn up" << std::endl;
        game.mouse.lButtonDown = false;
    } */

    case WM_MOUSEMOVE: {
        POINT mp;
        GetCursorPos(&mp);
        ScreenToClient(hwnd, &mp);

        game.mouse.pos.x = mp.x;
        game.mouse.pos.y = mp.y;

        return 0;
    }

    case WM_PAINT: {
        GameRender(hwnd);
        return 0;
    }

    default: {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"A* visualization";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Window Title",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);


    MSG msg = {};

    while (true)
    {
        // windows events
        // only process if not wm paint because wm paint gets triggered later at 60fps
        if (GetMessage(&msg, NULL, 0, 0) && msg.message != WM_PAINT) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        {
            game.mouse.lButtonDown = true;
        }
        else {
            game.mouse.lButtonDown = false;
        }

        if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
        {
            game.mouse.rButtonDown = true;
        }
        else {
            game.mouse.rButtonDown = false;
        }

        double elapsed = GetElapsedMilliseconds();
        if (elapsed >= DESIRED_FRAME_TIME)
        {
            lastFrameTime.QuadPart += static_cast<LONGLONG>(elapsed / DESIRED_FRAME_TIME) * static_cast<LONGLONG>(DESIRED_FRAME_TIME);

            GameUpdate();
            InvalidateRect(hwnd, NULL, TRUE);

            // Optional: Force an immediate repaint of the invalidated region
            UpdateWindow(hwnd);
        }
        else
        {
            // Sleep or yield to give other processes a chance to run
            Sleep(1);
        }
    }

    return 0;
}


int main(int argc, char* argv[])
{
    args::ArgumentParser parser("This program demonstrates argument parsing for rows, columns, size, and spacing.");

    args::HelpFlag help(parser, "help", "Display this help menu", { 'h', "help" });
    args::ValueFlag<int> rows(parser, "rows", "Number of rows (default: 10)", { 'r', "rows" });
    args::ValueFlag<int> cols(parser, "cols", "Number of columns (default: 10)", { 'c', "cols" });
    args::ValueFlag<int> size(parser, "size", "Size of each element (default: 50)", { 's', "size" });
    args::ValueFlag<int> spacing(parser, "spacing", "Spacing between elements (default: 2)", { 'p', "spacing" });

    try {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help&) {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    int numRows = rows ? *rows : 10;
    std::cout << "Number of rows: " << numRows << std::endl;

    int numCols = cols ? *cols : 10;
    std::cout << "Number of columns: " << numCols << std::endl;

    int elementSize = size ? *size : 50;
    std::cout << "Size of each element: " << elementSize << std::endl;

    int elementSpacing = spacing ? *spacing : 2;
    std::cout << "Spacing between elements: " << elementSpacing << std::endl;


    Grid grid = Grid(numRows, numCols, elementSize, elementSpacing);
    game = Game(grid);

    for (int row = 0; row < game.grid.rows; ++row)
    {
        for (int col = 0; col < game.grid.cols; ++col)
        {
            game.grid.setCell(row, col, GAME_AIR);
        }
    }
    game.grid.setCell(0, 0, GAME_START);
    game.grid.setCell(game.grid.rows-1, game.grid.cols-1, GAME_FINISH);

    
    // hide cmd
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_HIDE);

    HINSTANCE hInstance = GetModuleHandle(NULL);
    HINSTANCE hPrevInstance = NULL;
    LPSTR lpCmdLine = GetCommandLineA();
    int nCmdShow = SW_SHOWDEFAULT;


    return WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
