#include <Windows.h>
#include <iostream>
#include <string>
#include <chrono>
#include "Game.h"
#include "Grid.h"

constexpr int TARGET_FPS = 60;
constexpr double DESIRED_FRAME_TIME = 1000.0 / TARGET_FPS;
constexpr int GAME_AIR = 0;
constexpr int GAME_WALL = 1;
constexpr int gridCols = 10;
constexpr int gridRows = 10;
constexpr int squareSize = 50;
constexpr int squareSpacing = 2;

enum Types : int {
    AIR = 0,
    WALL = 1
};

LARGE_INTEGER lastFrameTime;
Game game = Game(Grid(0, 0, 0, 0));

COLORREF airColor = RGB(255, 255, 255);
COLORREF wallColor = RGB(0, 0, 0);

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
            int x1 = col * squareSize + col * squareSpacing;
            int y1 = row * squareSize + row * squareSpacing;
            int x2 = x1 + squareSize;
            int y2 = y1 + squareSize;

            switch (game.grid.getCell(row, col)) {

            case (GAME_AIR): 
                DrawRectangle(hdc, x1, y1, x2, y2, airColor);
            
            case (GAME_WALL):
                DrawRectangle(hdc, x1, y1, x2, y2, wallColor);

            }

        }
    }

}


bool GetMouseSquare(POINT* point)
{
    POINT square;
    square.x = game.mousePos.x / (game.grid.cols + game.grid.spacing);
    square.y = game.mousePos.y / (game.grid.rows + game.grid.spacing);

    if (0 <= square.x && square.x < game.grid.cols && 0 <= square.y && square.y < game.grid.rows ) {
        *point = square;
        return true;
    }

    return false;
}

void GameUpdate()
{
    // Update your game logic here
}

void GameRender(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    DrawGrid(hdc);

    POINT curSquare;
    /* if (GetMouseSquare(&curSquare)) {
        int x1 = curSquare.x * squareSize + curSquare.x * squareSpacing;
        int y1 = curSquare.y * squareSize + curSquare.y * squareSpacing;
        int x2 = x1 + squareSize;
        int y2 = y1 + squareSize;

        DrawRectangle(hdc, x1, y1, x2, y2, RGB(0, 0, 255));
    }*/

    //std::wstring posString = L"Mouse position: (" + std::to_wstring(game.mousePos.x) + L", " + std::to_wstring(game.mousePos.y) + L")";
    //TextOut(hdc, 10, 10, posString.c_str(), posString.length());

    EndPaint(hwnd, &ps);
    ReleaseDC(hwnd, hdc);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

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

    case WM_MOUSEMOVE: {
        GetCursorPos(&game.mousePos);
        ScreenToClient(hwnd, &game.mousePos);

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


int main()
{
    Grid grid = Grid(gridRows, gridCols, squareSize, squareSpacing);
    game = Game(grid);

    for (int row = 0; row < game.grid.rows; ++row)
    {
        for (int col = 0; col < game.grid.cols; ++col)
        {
            game.grid.setCell(row, col, GAME_AIR);
        }
    }

    HINSTANCE hInstance = GetModuleHandle(NULL);
    HINSTANCE hPrevInstance = NULL;
    LPSTR lpCmdLine = GetCommandLineA();
    int nCmdShow = SW_SHOWDEFAULT;


    return WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
