#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include <vector>
#include "AStar.h"

class Mouse {
public:
	Point pos;
	bool rButtonDown;
	bool lButtonDown;

    Mouse();
};


class Grid {
private:
    std::vector<std::vector<int>> grid;
    AStar path_finder;

public:
    int size;
    int rows;
    int cols;
    int spacing;
    bool changed = true;
    Point start;
    Point finish;

    Grid(int nRows, int nCols, int sqSize, int sqSpacing);
    void setCell(int row, int col, int value);
    int getCell(int row, int col) const;
    std::vector<Point> findPath();
};

class Game
{
public:
	Game(Grid g);
	Grid grid;
	Mouse mouse;
};

#endif // GRID_H
