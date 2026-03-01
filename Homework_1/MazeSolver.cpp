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

#include "MazeSolver.h"
#include "MazeGenerator.h"
#include <algorithm>
#include <limits>
#include <list>
#include <vector>

// Define static constexpr members of MazeSolver
constexpr int MazeSolver::DIRECTION_ROW[];
constexpr int MazeSolver::DIRECTION_COL[];
constexpr int MazeSolver::NUM_DIRECTIONS;

MazeSolver::MazeSolver(Maze &maze)
    : m_maze(maze), start{maze.getStart().first, maze.getStart().second}, end{maze.getEnd().first, maze.getEnd().second}, terminator{-1, -1}
{
    cameFrom[start] = terminator; // Sentinel value for start
}

void MazeSolver::reset()
{
    cameFrom.clear();
    cameFrom[start] = terminator;
}

std::list<Position> MazeSolver::reconstructPath()
{
    std::list<Position> path;
    Position current = end;

    while (current != terminator)
    {
        path.push_back(current);
        current = cameFrom[current];

        // Mark solution path for visualization
        if (current != start)
        { // Avoid overwriting start cell
            m_maze.setCellType(current.row, current.col, CellType::Solution);
        }
    }

    return path;
}

BreadthFirstSearch::BreadthFirstSearch(Maze &maze) : MazeSolver(maze)
{
    // Initialize 2D visited array with maze dimensions, all false
    visited.assign(maze.getHeight(), std::vector<bool>(maze.getWidth(), false));
    frontier.push(start);
    visited[start.row][start.col] = true;
}

void BreadthFirstSearch::reset()
{
    MazeSolver::reset();

    // Clear all data structures
    while (!frontier.empty())
        frontier.pop();
    // Reset visited array instead of clearing (more efficient)
    for (auto &row : visited)
    {
        std::fill(row.begin(), row.end(), false);
    }

    // Reinitialize with start position
    frontier.push(start);
    visited[start.row][start.col] = true;
}

std::list<Position> BreadthFirstSearch::solveMaze()
{
    int nodesExploredCount = 0;
    while (!frontier.empty())
    {
        if (step(nodesExploredCount))
        {
            return reconstructPath(); // Mark the path on the maze
        }
    }

    return {}; // No path found
}

bool BreadthFirstSearch::step(int &nodesExploredCount)
{
    // check if search is complete
    if (frontier.empty())
    {
        return false; // No path found - all reachable cells explored
    }

    // process next cell
    Position current = frontier.front();
    frontier.pop();

    // Check if we've reached the goal
    if (current == end)
    {
        return true; // Path found!
    }

    // explore neighbors in order: UP, DOWN, LEFT, RIGHT
    for (int i = 0; i < NUM_DIRECTIONS; ++i)
    {
        Position next = {current.row + DIRECTION_ROW[i], current.col + DIRECTION_COL[i]};

        // Only visit unvisited, passable cells
        if (m_maze.isValidPath(next.row, next.col) && !visited[next.row][next.col])
        {
            visited[next.row][next.col] = true;
            cameFrom[next] = current; // Track path for reconstruction
            frontier.push(next);      // Add to queue for future exploration
            nodesExploredCount++;

            // Mark explored cells (not start/end) as "visited" for UI animation
            if (m_maze.getCell(next.row, next.col).type == CellType::Path)
            {
                m_maze.setCellType(next.row, next.col, CellType::Visited);
            }
        }
    }

    return false; // Continue searching
}

AStarSearch::AStarSearch(Maze &maze) : MazeSolver(maze)
{
    // Initialize 2D arrays with maze dimensions
    int height = maze.getHeight();
    int width = maze.getWidth();
    gScore.assign(height, std::vector<int>(width, std::numeric_limits<int>::max()));
    inOpenSet.assign(height, std::vector<bool>(width, false));

    openSet.push({start, manhattanDistance(start, end)});
    gScore[start.row][start.col] = 0;
    inOpenSet[start.row][start.col] = true;
}

void AStarSearch::reset()
{
    MazeSolver::reset();

    while (!openSet.empty())
        openSet.pop();

    // Reset 2D arrays instead of clearing
    for (auto &row : gScore)
    {
        std::fill(row.begin(), row.end(), std::numeric_limits<int>::max());
    }
    for (auto &row : inOpenSet)
    {
        std::fill(row.begin(), row.end(), false);
    }

    gScore[start.row][start.col] = 0;
    openSet.push({start, manhattanDistance(start, end)});
    inOpenSet[start.row][start.col] = true;
}

std::list<Position> AStarSearch::solveMaze()
{
    int nodesExploredCount = 0;

    while (!openSet.empty())
    {
        if (step(nodesExploredCount))
        {
            return reconstructPath(); // Mark the path on the maze
        }
    }

    return {}; // No path found
}

bool AStarSearch::step(int &nodesExploredCount)
{
    // check if search is complete
    if (openSet.empty())
    {
        return false; // No path found - no more cells to explore
    }

    // Process most promising cell from open set (lowest f-score)
    // Priority queue gives us the cell with lowest f-score (best guess at shortest path)
    Node current = openSet.top();
    openSet.pop();
    inOpenSet[current.pos.row][current.pos.col] = false;

    // Check if we've reached the goal
    if (current.pos == end)
    {
        return true; // Shortest path found!
    }

    // A* explores in order: UP, DOWN, LEFT, RIGHT
    for (int i = 0; i < NUM_DIRECTIONS; ++i)
    {
        Position next = {current.pos.row + DIRECTION_ROW[i], current.pos.col + DIRECTION_COL[i]};

        // Only consider valid, passable cells
        if (m_maze.isValidPath(next.row, next.col))
        {
            nodesExploredCount++;

            // Calculate path cost
            // g-score: actual distance from start to next node
            int tentativeG = gScore[current.pos.row][current.pos.col] + 1;

            // Check if this is a new cell or if we found a better path to it
            if (gScore[next.row][next.col] == std::numeric_limits<int>::max() || tentativeG < gScore[next.row][next.col])
            {
                // Record this as the best path to 'next' so far
                cameFrom[next] = current.pos;
                gScore[next.row][next.col] = tentativeG;

                // Calculate heuristic
                // f-score: estimated total cost = actual cost + Manhattan distance to goal
                int fScore = tentativeG + manhattanDistance(next, end);

                // Add to open set only if not already there
                if (!inOpenSet[next.row][next.col])
                {
                    openSet.push({next, fScore});
                    inOpenSet[next.row][next.col] = true;

                    // Mark explored cells (not start/end) as "visited" for UI animation
                    if (m_maze.getCell(next.row, next.col).type == CellType::Path)
                    {
                        m_maze.setCellType(next.row, next.col, CellType::Visited);
                    }
                }
            }
        }
    }

    return false; // Continue searching
}

int AStarSearch::manhattanDistance(const Position a, const Position b)
{
    return std::abs(a.row - b.row) + std::abs(a.col - b.col);
}
