/**
 * @file MazeGenerator.h
 * @brief Maze generation using recursive backtracking algorithm
 * @course ECE 4122/6122 - Homework 1
 * 
 * This header defines the Maze class which generates random, solvable mazes
 * using the recursive backtracking algorithm.
 */

#ifndef MAZE_GENERATOR_H
#define MAZE_GENERATOR_H

#include <vector>
#include <random>
#include <stack>
#include <utility>
#include <SFML/Graphics.hpp>

/**
 * @enum CellType
 * @brief Enumeration of possible cell states in the maze
 */
enum class CellType 
{
    Wall,       ///< Impassable wall
    Path,       ///< Open path that can be traversed
    Start,      ///< Starting position
    End,        ///< Goal/ending position
    Visited,    ///< Cell visited during pathfinding (for visualization)
    Solution    ///< Cell on the shortest path (for visualization)
};

/**
 * @struct Cell
 * @brief Represents a single cell in the maze grid
 */
struct Cell 
{
    CellType type;
    bool visited;  // Used during maze generation
    int row, col;
    
    Cell() : type(CellType::Wall), visited(false), row(0), col(0) {}
};

/**
 * @class Maze
 * @brief Generates and manages a 2D maze
 * 
 * The maze is represented as a 2D grid where each cell can be either
 * a wall or a path. The generation algorithm ensures there is always
 * a valid path from start to end.
 */
class Maze 
{
public:
    /**
     * @brief Construct a new Maze object
     * @param width Number of cells horizontally (will be adjusted to odd number)
     * @param height Number of cells vertically (will be adjusted to odd number)
     * @param seed Optional seed for random number generator (0 = time-based)
     */
    Maze(int width, int height, unsigned int seed = 0);
    
    /**
     * @brief Generate a new random maze
     * 
     * Uses recursive backtracking algorithm to create a perfect maze
     * (one with no loops and exactly one path between any two points)
     */
    void generate();

    // get the minimum window size needed to display the maze based on its dimensions
    void getWindowSize(int &width, int &height, int maxWidth, int maxHeight);

    /**
     * @brief Reset visualization markers (visited and solution cells)
     */
    void resetVisualization();
    
    /**
     * @brief Get the cell at a specific position
     * @param row Row index
     * @param col Column index
     * @return Reference to the cell
     */
    Cell& getCell(int row, int col);
    const Cell& getCell(int row, int col) const;
    
    /**
     * @brief Set the type of a cell
     * @param row Row index
     * @param col Column index
     * @param type New cell type
     */
    void setCellType(int row, int col, CellType type);
    
    /**
     * @brief Get maze dimensions
     */
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
    /**
     * @brief Get start and end positions
     */
    std::pair<int, int> getStart() const { return m_start; }
    std::pair<int, int> getEnd() const { return m_end; }
    
    /**
     * @brief Check if a position is valid and traversable
     * @param row Row index
     * @param col Column index
     * @return true if the cell can be traversed
     */
    bool isValidPath(int row, int col) const;
    
    /**
     * @brief Check if a position is within maze bounds
     */
    bool isInBounds(int row, int col) const;

    void draw(sf::RenderWindow& window);

private:
    int m_width;
    int m_height;
    int m_cell_size; // Size of each cell in pixels
    std::vector<std::vector<Cell>> m_grid;
    std::pair<int, int> m_start;
    std::pair<int, int> m_end;
    std::mt19937 m_rng; // Random number generator
    
    /**
     * @brief Initialize the grid with all walls
     */
    void initializeGrid();
    
    /**
     * @brief Recursive backtracking maze generation
     * @param row Current row
     * @param col Current column
     */
    void carvePassages(int row, int col);
    
    /**
     * @brief Get unvisited neighbors for maze generation
     * @param row Current row
     * @param col Current column
     * @return Vector of neighbor positions
     */
    std::vector<std::pair<int, int>> getUnvisitedNeighbors(int row, int col);
    
    void calculateWindowSize(int &window_width, int &window_height);
    void calculateAdjustedWindowAndCellSize(int &window_width, int &window_height);

    /**
     * @brief Place start and end positions
     */
    void placeStartAndEnd();
};

#endif // MAZE_GENERATOR_H
