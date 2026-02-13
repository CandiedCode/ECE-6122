#include "MazeSolver.h"
#include "MazeGenerator.h"
#include <list>

// Define static constexpr members of MazeSolver
constexpr int MazeSolver::DIRECTION_ROW[];
constexpr int MazeSolver::DIRECTION_COL[];
constexpr int MazeSolver::NUM_DIRECTIONS;

MazeSolver::MazeSolver(Maze &maze) : m_maze(maze)
{
    terminator = Position{-1, -1}; // Sentinel value for path reconstruction
    start = Position{maze.getStart().first, maze.getStart().second};
    end = Position{maze.getEnd().first, maze.getEnd().second};
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
    frontier.push(start);
    visited[start] = true;
}

void BreadthFirstSearch::reset()
{
    MazeSolver::reset();

    // Clear all data structures
    while (!frontier.empty())
        frontier.pop();
    visited.clear();

    // Reinitialize with start position
    frontier.push(start);
    visited[start] = true;
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

// @brief Perform one step of breadth-first search
// Processes the next cell in the frontier and explores its unvisited neighbors.
// For visualization, visited cells are marked with a light blue color.
// @param nodesExploredCount Reference to counter for visited nodes
// @return true if goal reached, false otherwise
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
        if (m_maze.isValidPath(next.row, next.col) && !visited[next])
        {
            visited[next] = true;
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
    openSet.push({start, manhattanDistance(start, end)});
    gScore[start] = 0;
    inOpenSet[start] = true;
}

void AStarSearch::reset()
{
    MazeSolver::reset();

    while (!openSet.empty())
        openSet.pop();
    gScore.clear();
    inOpenSet.clear();

    gScore[start] = 0;
    openSet.push({start, manhattanDistance(start, end)});
    inOpenSet[start] = true;
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

// @brief Perform one step of A* pathfinding
// Processes the cell with the lowest f-score (g + h) and explores neighbors.
// Uses Manhattan distance heuristic to guide search toward the goal.
// @param nodesExploredCount Reference to counter for evaluated nodes
// @return true if goal reached, false otherwise
bool AStarSearch::step(int &nodesExploredCount)
{
    // === CHECK IF SEARCH IS COMPLETE ===
    if (openSet.empty())
    {
        return false; // No path found - no more cells to explore
    }

    // Process most promising cell from open set (lowest f-score)
    // Priority queue gives us the cell with lowest f-score (best guess at shortest path)
    Node current = openSet.top();
    openSet.pop();
    inOpenSet[current.pos] = false;

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
            int tentativeG = gScore[current.pos] + 1;

            // Check if this is a new cell or if we found a better path to it
            if (gScore.find(next) == gScore.end() || tentativeG < gScore[next])
            {
                // Record this as the best path to 'next' so far
                cameFrom[next] = current.pos;
                gScore[next] = tentativeG;

                // Calculate heuristic
                // f-score: estimated total cost = actual cost + Manhattan distance to goal
                int fScore = tentativeG + manhattanDistance(next, end);

                // Add to open set only if not already there
                if (!inOpenSet[next])
                {
                    openSet.push({next, fScore});
                    inOpenSet[next] = true;

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

// @brief Manhattan distance heuristic for A* search
// @param a First position
// @param b Second position
// @return Manhattan distance between a and b
int AStarSearch::manhattanDistance(Position a, Position b)
{
    int score = std::abs(a.row - b.row) + std::abs(a.col - b.col);
    return score;
}
