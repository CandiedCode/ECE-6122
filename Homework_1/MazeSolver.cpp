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
    queue = new std::queue<Position>();
    visited = new std::unordered_set<Position, PositionHash>();
    parent = new std::unordered_map<Position, Position, PositionHash>();

    std::pair<int, int> startPos = maze.getStart();
    std::pair<int, int> endPos = maze.getEnd();
    terminator = Position{-1, -1}; // Sentinel value for path reconstruction
    start = Position{startPos.first, startPos.second};
    end = Position{endPos.first, endPos.second};

    (*parent)[start] = terminator;

    queue->push(start);
    visited->insert(start);
}

std::list<Position> BreadthFirstSearch::reconstructPath(std::unordered_map<Position, Position, PositionHash> &parent,
                                                        Position end)
{
    std::list<Position> path;
    Position current = end;

    while (current != terminator)
    {
        path.push_back(current);
        current = parent[current];
    }

    return path;
}

std::vector<Position> BreadthFirstSearch::solveMaze()
{
    std::queue<Position> frontier;
    std::unordered_map<Position, Position, PositionHash> cameFrom;
    std::unordered_map<Position, bool, PositionHash> visited;

    frontier.push(start);
    visited[start] = true;
    cameFrom[start] = terminator; // Sentinel value for start

    // Direction vectors: up, down, left, right
    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = {0, 0, -1, 1};

    while (!frontier.empty())
    {
        Position current = frontier.front();
        frontier.pop();

        // Are we at the end?
        if (current == end)
        {
            // Reconstruct path
            std::vector<Position> path;
            Position pos = end;
            while (pos != terminator)
            {
                path.push_back(pos);
                pos = cameFrom[pos];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        // Explore neighbors
        for (int i = 0; i < 4; ++i)
        {
            // {-1, 0} = up
            // {1, 0} = down
            // {0, -1} = left
            // {0, 1} = right
            Position next = {current.row + dr[i], current.col + dc[i]};

            if (m_maze.isValidPath(next.row, next.col) && !visited[next])
            {
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
    }

    return {}; // No path found
}

AStarSearch::AStarSearch(Maze &maze) : m_maze(maze)
{
    start = Position{maze.getStart().first, maze.getStart().second};
    end = Position{maze.getEnd().first, maze.getEnd().second};
}

std::vector<Position> AStarSearch::solveMaze()
{
    // Min-heap priority queue
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_map<Position, Position, PositionHash> cameFrom;
    std::unordered_map<Position, int, PositionHash> gScore;
    std::unordered_map<Position, bool, PositionHash> inOpenSet;
    Position end = this->end;

    auto heuristic = [&end](Position p) {
        return std::abs(p.row - end.row) + std::abs(p.col - end.col);
    };
    
    gScore[start] = 0;
    openSet.push({start, heuristic(start)});
    inOpenSet[start] = true;
    cameFrom[start] = {-1, -1};
    
    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = {0, 0, -1, 1};
    
    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();
        inOpenSet[current.pos] = false;
        
        if (current.pos == end) {
            // Reconstruct path
            std::vector<Position> path;
            Position pos = end;
            while (!(pos.row == -1 && pos.col == -1)) {
                path.push_back(pos);
                pos = cameFrom[pos];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        for (int i = 0; i < 4; ++i) {
            Position next = {current.pos.row + dr[i], current.pos.col + dc[i]};
            
            if (!m_maze.isValidPath(next.row, next.col)) {
                continue;
            }
            
            int tentativeG = gScore[current.pos] + 1;
            
            // If this path is better (or first path found)
            if (gScore.find(next) == gScore.end() || tentativeG < gScore[next]) {
                cameFrom[next] = current.pos;
                gScore[next] = tentativeG;
                int fScore = tentativeG + heuristic(next);
                
                if (!inOpenSet[next]) {
                    openSet.push({next, fScore});
                    inOpenSet[next] = true;
                    
                    // Mark for visualization
                    if (m_maze.getCell(next.row, next.col).type == CellType::Path) {
                        m_maze.setCellType(next.row, next.col, CellType::Visited);
                    }
                }
            }
        }
    }
    
    return {};  // No path found
}