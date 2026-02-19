/*
Author: Jennifer Cwagenberg
Class: ECE6122
Last Date Modified: 2026-02-13
Description:  Homework 1: Maze Generator and Solver Visualization

This file uses sample code provided in PathfindingNotes.md and makes slight modifications
to implement the required functionality for Homework 1.
The MazeSolver class is an abstract base class that defines the interface for maze solving algorithms.
The BreadthFirstSearch and AStarSearch classes inherit from MazeSolver and implement the BFS and A* algorithms, respectively.
*/

#ifndef HOMEWORK_1_MAZESOLVER_H_
#define HOMEWORK_1_MAZESOLVER_H_

#include "MazeGenerator.h"
#include <functional>
#include <list>
#include <queue>
#include <unordered_map>
#include <vector>

// Position in the maze
struct Position
{
    int row, col;

    bool operator==(const Position &other) const
    {
        return row == other.row && col == other.col;
    }
    bool operator!=(const Position &other) const
    {
        return !(*this == other);
    }
};

// For using Position as a key in unordered_map
struct PositionHash
{
    size_t operator()(const Position &p) const
    {
        return std::hash<int>()(p.row) ^ (std::hash<int>()(p.col) << 16);
    }
};

// Node structure for A* search
struct Node
{
    Position pos;
    int fScore;

    bool operator>(const Node &other) const
    {
        return fScore > other.fScore;
    }
};

// @class MazeSolver
// @brief Abstract base class for maze solving algorithms
class MazeSolver
{
  protected:
    // Direction constants for 4-directional pathfinding (UP, DOWN, LEFT, RIGHT)
    static constexpr int DIRECTION_ROW[] = {-1, 1, 0, 0};
    static constexpr int DIRECTION_COL[] = {0, 0, -1, 1};
    static constexpr int NUM_DIRECTIONS = 4;

    Maze &m_maze;
    Position start;
    Position end;
    Position terminator;
    std::unordered_map<Position, Position, PositionHash> cameFrom;

  public:
    /**
     * @brief Construct a new MazeSolver object
     * @param maze Reference to the maze to be solved
     */
    explicit MazeSolver(Maze &maze);
    virtual ~MazeSolver() = default;
    /**
     * @brief Solve the maze using the specific algorithm, returning the path from start to end as a list of positions
     * @return List of positions representing the path from start to end (empty if no path found)
     */
    virtual std::list<Position> solveMaze() = 0;
    /**
     * @brief Perform one step of the algorithm, exploring the next node and updating the search state accordingly
     * @param nodesExploredCount Reference to the integer counting the number of nodes explored so far (incremented when a node is
     * processed)
     * @return true if the goal has been reached and the path can be reconstructed
     */
    virtual bool step(int &nodesExploredCount) = 0;
    /**
     * @brief Reconstruct the path from start to end after the search is complete
     * @return List of positions representing the reconstructed path
     */
    virtual std::list<Position> reconstructPath();
    /**
     * @brief Reset the solver to its initial state, clearing all data structures and reinitializing the start position
     */
    virtual void reset();
};

// @class BreadthFirstSearch
// @brief Implements BFS algorithm for maze solving
class BreadthFirstSearch : public MazeSolver
{
  public:
    /**
     * @brief Construct a new BFS solver object
     * @param maze Reference to the maze to be solved
     */
    explicit BreadthFirstSearch(Maze &maze);
    /**
     * @brief Solve the maze using breadth-first search, returning the path from start to end as a list of positions
     * @return List of positions representing the path from start to end (empty if no path found)
     */
    std::list<Position> solveMaze() override;
    /**
     * @brief Perform one step of the BFS algorithm, exploring the next node in the frontier and updating the search state accordingly
     * @param nodesExploredCount Reference to the integer counting the number of nodes explored so far (incremented when a node is
     * processed)
     * @return true if the goal has been reached and the path can be reconstructed
     */
    bool step(int &nodesExploredCount) override;
    /**
     * @brief Reset the BFS solver to its initial state, clearing all data structures and reinitializing the frontier with the start
     * position
     */
    void reset() override;

  private:
    std::queue<Position> frontier;
    std::unordered_map<Position, bool, PositionHash> visited;
};

// @class AStarSearch
// @brief Implements A* algorithm for maze solving
class AStarSearch : public MazeSolver
{
  public:
    /**
     * @brief Construct a new A* solver object
     * @param maze Reference to the maze to be solved
     */
    explicit AStarSearch(Maze &maze);

    /**
     * @brief Solve the maze using A* algorithm, returning the path from start to end as a list of positions
     * @return List of positions representing the path from start to end (empty if no path found)
     */
    std::list<Position> solveMaze() override;
    /**
     * @brief Perform one step of the A* algorithm, exploring the next node in the open set and updating the search state accordingly
     * @param nodesExploredCount Reference to the integer counting the number of nodes explored so far (incremented when a node is
     * processed)
     * @return true if the goal has been reached and the path can be reconstructed
     */
    bool step(int &nodesExploredCount) override;

    /**
     * @brief Reset the A* solver to its initial state, clearing all data structures and reinitializing the open set with the start position
     */
    void reset() override;

  private:
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_map<Position, int, PositionHash> gScore;
    std::unordered_map<Position, bool, PositionHash> inOpenSet;

    /** @brief Manhattan distance heuristic for A* search
     *  @param a First position
     *  @param b Second position
     *  @return Manhattan distance between a and b
     */
    static int manhattanDistance(Position a, Position b);
};

#endif // HOMEWORK_1_MAZESOLVER_H_
