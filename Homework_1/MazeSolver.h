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


class BreadthFirstSearch {
public:
    BreadthFirstSearch(Maze& maze);
    BreadthFirstSearch& operator=(const BreadthFirstSearch&) = default;
    BreadthFirstSearch& operator=(BreadthFirstSearch&&) = default; 

    std::list<Position> reconstructPath(std::unordered_map<Position, Position, PositionHash>& parent, Position end);
    std::vector<Position> solveMaze(Maze &maze);


private:
    Position start;
    Position end;
    Position terminator;
    std::queue<Position>* queue;
    std::unordered_set<Position, PositionHash>* visited;
    std::unordered_map<Position, Position, PositionHash>* parent; // For path reconstruction

};

// class AStarSearch  {
// public:
//     int manhattanDistance(Position a, Position b);
//     int euclideanDistance(Position a, Position b);
//     int chebyshevDistance(Position a, Position b);
//     void solveMaze(int maze[5][5], int startX, int startY, int endX, int endY) override;
// };

#endif // MAZE_SOLVER_H