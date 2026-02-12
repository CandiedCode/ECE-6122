#include "MazeSolver.h"
#include "MazeGenerator.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <list>
#include <queue>
#include <unordered_map>
#include <vector>

BreadthFirstSearch::BreadthFirstSearch(Maze &maze) : m_maze(maze)
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

void BreadthFirstSearch::reset() {
    // Clear all data structures
    while (!frontier.empty()) frontier.pop();
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
    }

    return path;
}

std::vector<Position> BreadthFirstSearch::solveMaze()
{
    while (!frontier.empty())
    {
        if (step()) {
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

bool BreadthFirstSearch::step()
{
    if (frontier.empty()) {
        return true; // No more steps possible
    }

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
    for (int i = 0; i < 4; ++i) {
        Position next = {current.row + DR[i], current.col + DC[i]};
        
        if (m_maze.isValidPath(next.row, next.col) && !visited[next]) {
            visited[next] = true;
            cameFrom[next] = current;
            frontier.push(next);
            
            // Mark for visualization (optional)
            if (m_maze.getCell(next.row, next.col).type == CellType::Path) 
            {
                m_maze.setCellType(next.row, next.col, CellType::Visited);
            }
        }
    }

    return false; // Path not found yet
}

AStarSearch::AStarSearch(Maze &maze) : m_maze(maze)
{
    start = Position{maze.getStart().first, maze.getStart().second};
    end = Position{maze.getEnd().first, maze.getEnd().second};
}

void AStarSearch::reset() {
    // Clear any existing data structures if needed
}

std::vector<Position> AStarSearch::solveMaze()
{
    // Min-heap priority queue
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_map<Position, Position, PositionHash> cameFrom;
    std::unordered_map<Position, int, PositionHash> gScore;
    std::unordered_map<Position, bool, PositionHash> inOpenSet;
    Position end = this->end;

    auto heuristic = [&end](Position p) { return std::abs(p.row - end.row) + std::abs(p.col - end.col); };

    gScore[start] = 0;
    openSet.push({start, heuristic(start)});
    inOpenSet[start] = true;
    cameFrom[start] = {-1, -1};

    while (!openSet.empty())
    {
        Node current = openSet.top();
        openSet.pop();
        inOpenSet[current.pos] = false;

        if (current.pos == end)
        {
            // Reconstruct path
            std::vector<Position> path;
            Position pos = end;
            while (!(pos.row == -1 && pos.col == -1))
            {
                path.push_back(pos);
                pos = cameFrom[pos];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (int i = 0; i < NUM_DIRECTIONS; ++i)
        {
            Position next = {current.pos.row + DR[i], current.pos.col + DC[i]};

            if (!m_maze.isValidPath(next.row, next.col))
            {
                continue;
            }

            int tentativeG = gScore[current.pos] + 1;

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
    }

    return {}; // No path found
}

bool AStarSearch::step()
{
    // Not implemented for stepwise execution in this example
    return true;
}