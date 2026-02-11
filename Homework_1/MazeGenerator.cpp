/**
 * @file MazeGenerator.cpp
 * @brief Implementation of maze generation using recursive backtracking
 * @course ECE 4122/6122 - Homework 21
 *
 * The recursive backtracking algorithm works as follows:
 * 1. Start with a grid full of walls
 * 2. Pick a starting cell and mark it as visited
 * 3. While there are unvisited cells:
 *    a. If current cell has unvisited neighbors:
 *       - Choose random unvisited neighbor
 *       - Remove wall between current and chosen cell
 *       - Move to chosen cell and mark as visited
 *       - Push current cell to stack
 *    b. Else if stack is not empty:
 *       - Pop a cell from stack and make it current
 * 4. Algorithm terminates when stack is empty
 */

#include "MazeGenerator.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

#define MAZE_OFFSET 10

Maze::Maze(int width, int height, unsigned int seed)
{
    // Ensure dimensions are odd for proper maze structure
    // (walls on even indices, paths on odd indices)
    m_width = (width % 2 == 0) ? width + 1 : width;
    m_height = (height % 2 == 0) ? height + 1 : height;
    m_cell_size = 12;

    // Enforce minimum and maximum sizes
    m_width = std::max(11, std::min(m_width, 101));
    m_height = std::max(11, std::min(m_height, 101));

    // Seed random number generator
    if (seed == 0)
    {
        // Use time-based seed if not provided
        auto time_seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        m_rng.seed(static_cast<unsigned int>(time_seed));
    }
    else
    {
        m_rng.seed(seed);
    }

    std::cout << "Initialized maze with dimensions: " << m_width << "x" << m_height << std::endl;

    initializeGrid();
}

void Maze::initializeGrid()
{
    m_grid.resize(m_height);
    for (int row = 0; row < m_height; ++row)
    {
        m_grid[row].resize(m_width);
        for (int col = 0; col < m_width; ++col)
        {
            m_grid[row][col].type = CellType::Wall;
            m_grid[row][col].visited = false;
            m_grid[row][col].row = row;
            m_grid[row][col].col = col;
        }
    }
}

void Maze::generate()
{
    // Reset the grid
    initializeGrid();

    // Start carving from position (1,1)
    // We use odd coordinates for paths, even for walls
    carvePassages(1, 1);

    // Place start and end positions
    placeStartAndEnd();
}

void Maze::getWindowSize(int &window_width, int &window_height, int desktopWidth, int desktopHeight)
{
    window_height = (m_height * m_cell_size) + (2 * MAZE_OFFSET);
    window_width = (m_width * m_cell_size) + (2 * MAZE_OFFSET);

    // std::cout << "Calculated maze window size: " << window_width << "x" << window_height << std::endl;
    // std::cout << "Desktop resolution: " << desktopWidth << "x" << desktopHeight << std::endl;

    if (window_width >= desktopWidth || window_height >= desktopHeight)
    {
        desktopHeight -= (2 * MAZE_OFFSET);
        desktopWidth -= (2 * MAZE_OFFSET);

        if (window_width >= desktopWidth)
        {
            window_width = desktopWidth;
        }
        if (window_height >= desktopHeight)
        {
            window_height = desktopHeight;
        }
        

        float heightRatio = (desktopHeight / m_height);
        float widthRatio = (desktopWidth / m_width);
        float cell_size = std::min(widthRatio, heightRatio);
        m_cell_size = static_cast<int>(cell_size);

        // std::cout << "Adjusted cell size: " << m_cell_size << " pixels" << std::endl;
        // std::cout << "Adjusted maze window size: " << window_width << "x" << window_height << std::endl;
        // std::cout << "Note: Maze may be scaled down to fit within desktop resolution." << std::endl;
    }
}

void Maze::draw(sf::RenderWindow &window)
{
    // offset values
    int offsetX = MAZE_OFFSET;
    int offsetY = MAZE_OFFSET;
    int cellSize = m_cell_size;

    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            Cell &cell = m_grid[y][x];

            // Draw rectangles for walls based on cell.walls booleans
            float left = offsetX + x * cellSize;
            float top = offsetY + y * cellSize;
            float right = left + cellSize;
            float bottom = top + cellSize;

            if (cell.type == CellType::Wall)
            {
                sf::RectangleShape wall(sf::Vector2f(cellSize, cellSize));
                wall.setPosition(left, top);
                wall.setFillColor(sf::Color(50, 50, 50)); // DarkGray
                window.draw(wall);
            }
            else if (cell.type == CellType::Path)
            {
                sf::RectangleShape visited(sf::Vector2f(cellSize, cellSize));
                visited.setPosition(left, top);
                visited.setFillColor(sf::Color(255, 255, 255)); // White
                visited.setOutlineThickness(1.f);
                visited.setOutlineColor(sf::Color(50, 50, 50)); // DarkGray border
                window.draw(visited);
            }
            else if (cell.type == CellType::Start)
            {
                sf::RectangleShape startMarker(sf::Vector2f(cellSize, cellSize));
                startMarker.setPosition(left, top);
                startMarker.setFillColor(sf::Color(0, 200, 0)); // Green
                window.draw(startMarker);
            }
            else if (cell.type == CellType::End)
            {
                sf::RectangleShape endMarker(sf::Vector2f(cellSize, cellSize));
                endMarker.setPosition(left, top);
                endMarker.setFillColor(sf::Color(200, 0, 0)); // Red
                window.draw(endMarker);
            }
        }
    }
}

void Maze::carvePassages(int row, int col)
{
    // Mark current cell as visited and carve it out
    m_grid[row][col].visited = true;
    m_grid[row][col].type = CellType::Path;

    // Get all unvisited neighbors (2 cells away to maintain wall structure)
    std::vector<std::pair<int, int>> neighbors = getUnvisitedNeighbors(row, col);

    // Shuffle neighbors for randomness
    std::shuffle(neighbors.begin(), neighbors.end(), m_rng);

    // Visit each neighbor
    for (const auto &neighbor : neighbors)
    {
        int nRow = neighbor.first;
        int nCol = neighbor.second;

        // Check if still unvisited (may have been visited via different path)
        if (!m_grid[nRow][nCol].visited)
        {
            // Carve through the wall between current and neighbor
            int wallRow = row + (nRow - row) / 2;
            int wallCol = col + (nCol - col) / 2;
            m_grid[wallRow][wallCol].type = CellType::Path;

            // Recursively carve from neighbor
            carvePassages(nRow, nCol);
        }
    }
}

std::vector<std::pair<int, int>> Maze::getUnvisitedNeighbors(int row, int col)
{
    std::vector<std::pair<int, int>> neighbors;

    // Check all four directions (2 cells away for wall structure)
    // Direction: {row_delta, col_delta}
    const int directions[4][2] = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}};

    for (const auto &dir : directions)
    {
        int newRow = row + dir[0];
        int newCol = col + dir[1];

        // Check bounds (staying within the border walls)
        if (newRow > 0 && newRow < m_height - 1 && newCol > 0 && newCol < m_width - 1)
        {
            if (!m_grid[newRow][newCol].visited)
            {
                // Valid unvisited neighbor
                neighbors.push_back({newRow, newCol});
            }
        }
    }

    return neighbors;
}

void Maze::placeStartAndEnd()
{
    // Place start in top-left area
    m_start = {1, 1};
    m_grid[m_start.first][m_start.second].type = CellType::Start;

    // Place end in bottom-right area
    m_end = {m_height - 2, m_width - 2};
    m_grid[m_end.first][m_end.second].type = CellType::End;
}

void Maze::resetVisualization()
{
    for (int row = 0; row < m_height; ++row)
    {
        for (int col = 0; col < m_width; ++col)
        {
            CellType type = m_grid[row][col].type;
            if (type == CellType::Visited || type == CellType::Solution)
            {
                m_grid[row][col].type = CellType::Path;
            }
        }
    }

    // Restore start and end markers
    m_grid[m_start.first][m_start.second].type = CellType::Start;
    m_grid[m_end.first][m_end.second].type = CellType::End;
}

Cell &Maze::getCell(int row, int col)
{
    return m_grid[row][col];
}

const Cell &Maze::getCell(int row, int col) const
{
    return m_grid[row][col];
}

void Maze::setCellType(int row, int col, CellType type)
{
    if (isInBounds(row, col))
    {
        m_grid[row][col].type = type;
    }
}

bool Maze::isInBounds(int row, int col) const
{
    return row >= 0 && row < m_height && col >= 0 && col < m_width;
}

bool Maze::isValidPath(int row, int col) const
{
    if (!isInBounds(row, col))
    {
        return false;
    }

    CellType type = m_grid[row][col].type;
    return type != CellType::Wall;
}

// ============================================================================
// ITERATIVE VERSION (Alternative implementation using explicit stack)
// ============================================================================
//
// For very large mazes, the recursive version may cause stack overflow.
// Here's an iterative alternative that you can use instead:
//
// void Maze::generateIterative()
//{
//     initializeGrid();
//
//     std::stack<std::pair<int, int>> cellStack;
//
//     // Start at (1, 1)
//     int currentRow = 1;
//     int currentCol = 1;
//     m_grid[currentRow][currentCol].visited = true;
//     m_grid[currentRow][currentCol].type = CellType::Path;
//
//     int visitedCount = 1;
//     int totalCells = ((m_height - 1) / 2) * ((m_width - 1) / 2);
//
//     while (visitedCount < totalCells) {
//         auto neighbors = getUnvisitedNeighbors(currentRow, currentCol);
//
//         if (!neighbors.empty()) {
//             // Choose random neighbor
//             std::uniform_int_distribution<int> dist(0, neighbors.size() - 1);
//             auto chosen = neighbors[dist(m_rng)];
//
//             // Push current cell to stack
//             cellStack.push({currentRow, currentCol});
//
//             // Remove wall between current and chosen
//             int wallRow = currentRow + (chosen.first - currentRow) / 2;
//             int wallCol = currentCol + (chosen.second - currentCol) / 2;
//             m_grid[wallRow][wallCol].type = CellType::Path;
//
//             // Move to chosen cell
//             currentRow = chosen.first;
//             currentCol = chosen.second;
//             m_grid[currentRow][currentCol].visited = true;
//             m_grid[currentRow][currentCol].type = CellType::Path;
//             visitedCount++;
//         }
//         else if (!cellStack.empty()) {
//             // Backtrack
//             auto cell = cellStack.top();
//             cellStack.pop();
//             currentRow = cell.first;
//             currentCol = cell.second;
//         }
//     }
//
//     placeStartAndEnd();
// }
