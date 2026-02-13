#include "MazeSolver.h"
#include "MazeGenerator.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <list>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

BreadthFirstSearch::BreadthFirstSearch(Maze &maze) : MazeSolver(maze)
{
    std::pair<int, int> startPos = maze.getStart();
    std::pair<int, int> endPos = maze.getEnd();
    terminator = Position{-1, -1}; // Sentinel value for path reconstruction
    start = Position{startPos.first, startPos.second};
    end = Position{endPos.first, endPos.second};

    frontier.push(start);
    visited[start] = true;
    cameFrom[start] = terminator; // Sentinel value for start
}

void BreadthFirstSearch::reset()
{
    // Clear all data structures
    while (!frontier.empty())
        frontier.pop();
    visited.clear();
    cameFrom.clear();

    // Reinitialize with start position
    frontier.push(start);
    visited[start] = true;
    cameFrom[start] = terminator;
}

std::list<Position> BreadthFirstSearch::reconstructPath()
{
    std::list<Position> path;
    Position current = end;

    while (current != terminator)
    {
        path.push_back(current);
        current = cameFrom[current];

        // Mark solution path for visualization
        if (current != start) // Avoid overwriting start cell
        {
            m_maze.setCellType(current.row, current.col, CellType::Solution);
        }
    }

    return path;
}

std::vector<Position> BreadthFirstSearch::solveMaze()
{
    int nodesExploredCount = 0;
    while (!frontier.empty())
    {
        if (step(nodesExploredCount))
        {
            // Path found, reconstruct it
            std::vector<Position> path;
            Position current = end;
            while (current != terminator)
            {
                path.push_back(current);
                current = cameFrom[current];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
    }

    return {}; // No path found
}

bool BreadthFirstSearch::step(int &nodesExploredCount)
{
    Position current = frontier.front();
    frontier.pop();

    // Are we at the end?
    if (current == end)
    {
        // Reconstruct path
        reconstructPath();
        return true;
    }

    // Explore neighbors
    for (int i = 0; i < NUM_DIRECTIONS; ++i)
    {
        Position next = {current.row + DR[i], current.col + DC[i]};

        if (m_maze.isValidPath(next.row, next.col) && !visited[next])
        {
            visited[next] = true;
            cameFrom[next] = current;
            frontier.push(next);
            nodesExploredCount++;

            // Mark for visualization (optional)
            if (m_maze.getCell(next.row, next.col).type == CellType::Path)
            {
                m_maze.setCellType(next.row, next.col, CellType::Visited);
            }
        }
    }

    return false; // Path not found yet
}

AStarSearch::AStarSearch(Maze &maze) : MazeSolver(maze)
{
    terminator = Position{-1, -1}; // Sentinel value for path reconstruction
    start = Position{maze.getStart().first, maze.getStart().second};
    end = Position{maze.getEnd().first, maze.getEnd().second};
    heuristic = [this](Position p) { return std::abs(p.row - end.row) + std::abs(p.col - end.col); };
    gScore[start] = 0;
    openSet.push({start, heuristic(start)});
    inOpenSet[start] = true;
    cameFrom[start] = terminator;
}

std::list<Position> AStarSearch::reconstructPath()
{
    // Reconstruct path
    std::list<Position> path;
    Position pos = end;
    while (!(pos == terminator))
    {
        path.push_back(pos);
        pos = cameFrom[pos];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

void AStarSearch::reset()
{
    while (!openSet.empty())
        openSet.pop();
    gScore.clear();
    cameFrom.clear();
    inOpenSet.clear();

    gScore[start] = 0;
    openSet.push({start, heuristic(start)});
    inOpenSet[start] = true;
    cameFrom[start] = terminator;
}

std::vector<Position> AStarSearch::solveMaze()
{
    int nodesExploredCount = 0;

    while (!openSet.empty())
    {
        std::cout << "Open set size: " << openSet.size() << std::endl; // Debugging output
        if (step(nodesExploredCount))
        {
            std::cout << "Path found with " << nodesExploredCount << " nodes explored." << std::endl; // Debugging output
            // Path found, reconstruct it
            std::vector<Position> path;
            Position current = end;
            while (current != terminator)
            {
                path.push_back(current);
                current = cameFrom[current];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
    }

    return {}; // No path found
}

bool AStarSearch::step(int &nodesExploredCount)
{
    Node current = openSet.top();
    openSet.pop();
    inOpenSet[current.pos] = false;

    std::cout << "Exploring node: (" << current.pos.row << ", " << current.pos.col << ") with fScore: " << current.fScore
              << std::endl; // Debugging output
    if (current.pos == end)
    {
        reconstructPath();
        return true;
    }

    for (int i = 0; i < NUM_DIRECTIONS; ++i)
    {
        Position next = {current.pos.row + DR[i], current.pos.col + DC[i]};

        if (!m_maze.isValidPath(next.row, next.col))
        {
            return false; // Skip invalid paths
        }

        int tentativeG = gScore[current.pos] + 1;
        nodesExploredCount++;

        // If this path is better (or first path found)
        if (gScore.find(next) == gScore.end() || tentativeG < gScore[next])
        {
            cameFrom[next] = current.pos;
            gScore[next] = tentativeG;
            int fScore = tentativeG + heuristic(next);

            if (!inOpenSet[next])
            {
                openSet.push({next, fScore});
                inOpenSet[next] = true;

                // Mark for visualization
                if (m_maze.getCell(next.row, next.col).type == CellType::Path)
                {
                    m_maze.setCellType(next.row, next.col, CellType::Visited);
                }
            }
        }
    }

    return false; // Path not found yet
}
