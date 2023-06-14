#include "Game.h"
#include <stdexcept>
#include <iostream>

Mouse::Mouse() {
    pos.x = 0;
    pos.y = 0;
    rButtonDown = false;
    lButtonDown = false;
}

Grid::Grid(int nRows, int nCols, int sqSize, int sqSpacing) : rows(nRows), cols(nCols), size(sqSize), spacing(sqSpacing) {
    // Initialize the grid with 0s
    grid.resize(rows, std::vector<int>(cols, 0));
    start.x = 0;
    start.y = 0;
    finish.x = nCols - 1;
    finish.y = nRows - 1;
}

void Grid::setCell(int row, int col, int value) {
    // Check if the given row and column are within bounds
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        // Check if the value is valid (0, 1, or 2)
        grid[row][col] = value;
    }
    else {
        // Invalid row or column
        throw std::out_of_range("Invalid cell position");
    }
}

int Grid::getCell(int row, int col) const {
    // Check if the given row and column are within bounds
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return grid[row][col];
    }
    else {
        // Invalid row or column
        throw std::out_of_range("Invalid cell position");
    }
}

std::vector<Point> Grid::findPath() {
    return path_finder.findPath(grid, start, finish);
}


Game::Game(Grid g) : grid(g) {

}

