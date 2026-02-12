#ifndef MAZE_SOLVER_H
#define MAZE_SOLVER_H

#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "MazeGenerator.h"
#include <list>

namespace {
      constexpr int DR[] = {-1, 1, 0, 0};
      constexpr int DC[] = {0, 0, -1, 1};
      constexpr int NUM_DIRECTIONS = 4;
}

// Position in the maze
struct Position {
    int row, col;
    
    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

// For using Position as a key in unordered_map
struct PositionHash {
    size_t operator()(const Position& p) const 
    {
        return std::hash<int>()(p.row) ^ (std::hash<int>()(p.col) << 16);
    }
};

struct Node {
    Position pos;
    int fScore;
    
    bool operator>(const Node& other) const {
        return fScore > other.fScore;
    }
};

class MazeSolver {
public:
    virtual std::vector<Position> solveMaze() = 0;
};

class BreadthFirstSearch : public MazeSolver {
public:
    BreadthFirstSearch(Maze& maze);

    std::list<Position> reconstructPath(std::unordered_map<Position, Position, PositionHash>& parent, Position end);
    std::vector<Position> solveMaze() override;


private:
    Maze& m_maze;
    Position start;
    Position end;
    Position terminator;
    std::queue<Position> queue;
    std::unordered_set<Position, PositionHash> visited;
    std::unordered_map<Position, Position, PositionHash> parent; // For path reconstruction

};

class AStarSearch: public MazeSolver {
public:
    AStarSearch(Maze& maze);
    
    int manhattanDistance(Position a, Position b);
    int euclideanDistance(Position a, Position b);
    int chebyshevDistance(Position a, Position b);
    std::vector<Position> solveMaze() override;

private:
    Maze& m_maze;
    Position start;
    Position end;
    Position terminator;
};

#endif // MAZE_SOLVER_H