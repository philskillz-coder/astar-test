#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include <vector>

class Mouse {
public:
	POINT pos;
	bool rButtonDown;
	bool lButtonDown;

    Mouse();
};


class Grid {
public:
    std::vector<std::vector<int>> grid;
    int size;
    int rows;
    int cols;
    int spacing;
    POINT start;
    POINT finish;

    Grid(int nRows, int nCols, int sqSize, int sqSpacing);
    void setCell(int row, int col, int value);
    int getCell(int row, int col) const;
};

class Game
{
public:
	Game(Grid g);
	Grid grid;
	Mouse mouse;
};

#endif // GRID_H
