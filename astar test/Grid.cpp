#include "Grid.h"
#include <stdexcept>

Grid::Grid(int nRows, int nCols, int sqSize, int sqSpacing) : rows(nRows), cols(nCols), size(sqSize), spacing(sqSpacing) {
    // Initialize the grid with 0s
    grid.resize(rows, std::vector<int>(cols, 0));
}

void Grid::setCell(int row, int col, int value) {
    // Check if the given row and column are within bounds
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        // Check if the value is valid (0, 1, or 2)
        if (value >= 0 && value <= 2) {
            grid[row][col] = value;
        }
        else {
            // Invalid value
            throw std::invalid_argument("Invalid cell value");
        }
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
