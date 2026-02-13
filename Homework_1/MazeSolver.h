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

struct Node
{
    Position pos;
    int fScore;

    bool operator>(const Node &other) const
    {
        return fScore > other.fScore;
    }
};

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
    explicit MazeSolver(Maze &maze);
    virtual ~MazeSolver() = default;
    virtual std::list<Position> solveMaze() = 0;
    virtual bool step(int &nodesExploredCount) = 0;
    virtual std::list<Position> reconstructPath();
    virtual void reset();
};

class BreadthFirstSearch : public MazeSolver
{
  public:
    explicit BreadthFirstSearch(Maze &maze);

    std::list<Position> solveMaze() override;
    bool step(int &nodesExploredCount) override;
    void reset() override;

  private:
    std::queue<Position> frontier;
    std::unordered_map<Position, bool, PositionHash> visited;
};

class AStarSearch : public MazeSolver
{
  public:
    explicit AStarSearch(Maze &maze);

    std::list<Position> solveMaze() override;
    bool step(int &nodesExploredCount) override;
    void reset() override;

  private:
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_map<Position, int, PositionHash> gScore;
    std::unordered_map<Position, bool, PositionHash> inOpenSet;

    int manhattanDistance(Position a, Position b);
};

#endif // HOMEWORK_1_MAZESOLVER_H_
