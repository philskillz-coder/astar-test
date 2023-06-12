#ifndef GRID_H
#define GRID_H

#include <vector>

class Grid {
private:
    std::vector<std::vector<int>> grid;

public:
    int size;
    int rows;
    int cols;
    int spacing;

    Grid(int nRows, int nCols, int sqSize, int sqSpacing);
    void setCell(int row, int col, int value);
    int getCell(int row, int col) const;
};

#endif // GRID_H
