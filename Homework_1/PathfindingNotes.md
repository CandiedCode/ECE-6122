# Pathfinding Algorithm Notes for ECE 4122/6122 Homework 1

## Overview

This document covers the two most common algorithms for finding the shortest path in a maze:
1. **Breadth-First Search (BFS)** - Guaranteed shortest path for unweighted graphs
2. **A\* (A-Star) Algorithm** - Optimal path with heuristic optimization

Both algorithms are well-suited for grid-based mazes where movement costs are uniform.

---

## Breadth-First Search (BFS)

### Concept

BFS explores the maze level by level, visiting all cells at distance 1 from the start, then all cells at distance 2, and so on. This guarantees finding the shortest path in an unweighted graph.

### Key Properties
- **Time Complexity**: O(V + E) where V = vertices, E = edges
- **Space Complexity**: O(V) for the queue and visited set
- **Guarantee**: Always finds the shortest path (for unweighted graphs)
- **Best for**: Mazes where all movements have equal cost

### Data Structures Needed
```cpp
#include <queue>
#include <unordered_map>
#include <vector>

// Position in the maze
struct Position {
    int row, col;
    
    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};

// For using Position as a key in unordered_map
struct PositionHash {
    size_t operator()(const Position& p) const {
        return std::hash<int>()(p.row) ^ (std::hash<int>()(p.col) << 16);
    }
};
```

### BFS Pseudocode

```
function BFS(maze, start, end):
    // Queue stores positions to explore
    queue = new Queue()
    queue.enqueue(start)
    
    // Track visited cells and their parents (for path reconstruction)
    visited = new Set()
    parent = new Map()  // Maps each cell to its predecessor
    
    visited.add(start)
    parent[start] = null
    
    while queue is not empty:
        current = queue.dequeue()
        
        // Check if we've reached the goal
        if current == end:
            return reconstructPath(parent, end)
        
        // Explore all valid neighbors (up, down, left, right)
        for each neighbor in getNeighbors(current):
            if neighbor not in visited AND maze.isValidPath(neighbor):
                visited.add(neighbor)
                parent[neighbor] = current
                queue.enqueue(neighbor)
                
                // Optional: Mark as visited for visualization
                maze.setCellType(neighbor, VISITED)
    
    // No path found
    return empty path

function reconstructPath(parent, end):
    path = new List()
    current = end
    
    while current is not null:
        path.prepend(current)
        current = parent[current]
    
    return path
```

### C++ Implementation Skeleton

```cpp
#include <queue>
#include <unordered_map>
#include <vector>
#include <algorithm>

std::vector<Position> solveBFS(Maze& maze) {
    Position start = {maze.getStart().first, maze.getStart().second};
    Position end = {maze.getEnd().first, maze.getEnd().second};
    
    std::queue<Position> frontier;
    std::unordered_map<Position, Position, PositionHash> cameFrom;
    std::unordered_map<Position, bool, PositionHash> visited;
    
    frontier.push(start);
    visited[start] = true;
    cameFrom[start] = {-1, -1};  // Sentinel value for start
    
    // Direction vectors: up, down, left, right
    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = {0, 0, -1, 1};
    
    while (!frontier.empty()) {
        Position current = frontier.front();
        frontier.pop();
        
        // Goal check
        if (current == end) {
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
        
        // Explore neighbors
        for (int i = 0; i < 4; ++i) {
            Position next = {current.row + dr[i], current.col + dc[i]};
            
            if (maze.isValidPath(next.row, next.col) && !visited[next]) {
                visited[next] = true;
                cameFrom[next] = current;
                frontier.push(next);
                
                // Mark for visualization (optional)
                if (maze.getCell(next.row, next.col).type == CellType::Path) {
                    maze.setCellType(next.row, next.col, CellType::Visited);
                }
            }
        }
    }
    
    return {};  // No path found
}
```

---

## A\* Algorithm

### Concept

A\* combines the benefits of Dijkstra's algorithm (guaranteed shortest path) with a heuristic function that guides the search toward the goal. It uses:
- **g(n)**: Actual cost from start to node n
- **h(n)**: Estimated cost from node n to goal (heuristic)
- **f(n) = g(n) + h(n)**: Total estimated cost

### Key Properties
- **Time Complexity**: O(E log V) with good heuristic, worse without
- **Space Complexity**: O(V)
- **Guarantee**: Optimal if heuristic is admissible (never overestimates)
- **Best for**: Large mazes where BFS explores too many cells

### Heuristic Functions

For grid-based mazes, the **Manhattan distance** is the standard heuristic:

```cpp
int manhattanDistance(Position a, Position b) {
    return std::abs(a.row - b.row) + std::abs(a.col - b.col);
}
```

If diagonal movement is allowed, use **Euclidean distance** or **Chebyshev distance**:

```cpp
// Euclidean (for diagonal movement)
double euclideanDistance(Position a, Position b) {
    int dr = a.row - b.row;
    int dc = a.col - b.col;
    return std::sqrt(dr * dr + dc * dc);
}

// Chebyshev (diagonal costs same as cardinal)
int chebyshevDistance(Position a, Position b) {
    return std::max(std::abs(a.row - b.row), std::abs(a.col - b.col));
}
```

### A\* Pseudocode

```
function AStar(maze, start, end):
    // Priority queue ordered by f-score (lowest first)
    openSet = new PriorityQueue()
    openSet.add(start, priority = h(start, end))
    
    // Track the best path to each node
    cameFrom = new Map()
    
    // g-score: cost from start to this node
    gScore = new Map(default = infinity)
    gScore[start] = 0
    
    // f-score: estimated total cost through this node
    fScore = new Map(default = infinity)
    fScore[start] = h(start, end)
    
    while openSet is not empty:
        current = openSet.extractMin()  // Node with lowest f-score
        
        if current == end:
            return reconstructPath(cameFrom, end)
        
        for each neighbor in getNeighbors(current):
            if not maze.isValidPath(neighbor):
                continue
                
            // Cost to reach neighbor through current
            tentativeG = gScore[current] + 1  // +1 for uniform cost
            
            if tentativeG < gScore[neighbor]:
                // This path is better
                cameFrom[neighbor] = current
                gScore[neighbor] = tentativeG
                fScore[neighbor] = tentativeG + h(neighbor, end)
                
                if neighbor not in openSet:
                    openSet.add(neighbor, priority = fScore[neighbor])
                    
                    // Optional: Mark for visualization
                    maze.setCellType(neighbor, VISITED)
    
    return empty path  // No path exists
```

### C++ Implementation Skeleton

```cpp
#include <queue>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <limits>

struct Node {
    Position pos;
    int fScore;
    
    bool operator>(const Node& other) const {
        return fScore > other.fScore;
    }
};

std::vector<Position> solveAStar(Maze& maze) {
    Position start = {maze.getStart().first, maze.getStart().second};
    Position end = {maze.getEnd().first, maze.getEnd().second};
    
    // Min-heap priority queue
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    
    std::unordered_map<Position, Position, PositionHash> cameFrom;
    std::unordered_map<Position, int, PositionHash> gScore;
    std::unordered_map<Position, bool, PositionHash> inOpenSet;
    
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
            
            if (!maze.isValidPath(next.row, next.col)) {
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
                    if (maze.getCell(next.row, next.col).type == CellType::Path) {
                        maze.setCellType(next.row, next.col, CellType::Visited);
                    }
                }
            }
        }
    }
    
    return {};  // No path found
}
```

---

## Comparison: BFS vs A\*

| Aspect | BFS | A\* |
|--------|-----|-----|
| Optimality | Always optimal | Optimal with admissible heuristic |
| Cells Explored | More (explores uniformly) | Fewer (guided by heuristic) |
| Implementation | Simpler | More complex |
| Memory | Queue + visited set | Priority queue + multiple maps |
| Best Use Case | Small/medium mazes | Large mazes |

### Visual Example

Consider this 7x7 maze:
```
# # # # # # #
# S . . . . #
# # # # . # #
# . . . . . #
# . # # # # #
# . . . . E #
# # # # # # #
```

**BFS Exploration** (explores level by level):
- Explores all cells at distance 1 from S
- Then all cells at distance 2
- Continues until finding E

**A\* Exploration** (guided by Manhattan distance):
- Prioritizes cells that appear closer to E
- May skip exploring "dead ends" away from E
- Generally explores fewer cells

---

## Animation Tips for SFML

### Step-by-Step Visualization

Instead of solving instantly, you can animate the search:

```cpp
class PathfindingSolver {
private:
    std::queue<Position> frontier;  // or priority_queue for A*
    // ... other data structures
    
public:
    // Call once per frame to advance one step
    bool step() {
        if (frontier.empty()) {
            return true;  // Done (no path found)
        }
        
        Position current = frontier.front();
        frontier.pop();
        
        if (current == end) {
            reconstructPath();
            return true;  // Done (path found)
        }
        
        // Process neighbors...
        // Mark cells as visited
        
        return false;  // Not done yet
    }
};

// In your game loop:
void update(sf::Time deltaTime) {
    static sf::Time accumulated;
    accumulated += deltaTime;
    
    if (accumulated >= animationSpeed && solving) {
        accumulated = sf::Time::Zero;
        if (solver.step()) {
            solving = false;
        }
    }
}
```

### Color Interpolation for Smooth Transitions

```cpp
sf::Color lerpColor(sf::Color a, sf::Color b, float t) {
    return sf::Color(
        static_cast<sf::Uint8>(a.r + t * (b.r - a.r)),
        static_cast<sf::Uint8>(a.g + t * (b.g - a.g)),
        static_cast<sf::Uint8>(a.b + t * (b.b - a.b))
    );
}
```

---

## Common Mistakes to Avoid

1. **Not tracking visited cells**: Results in infinite loops
2. **Checking goal only when adding to queue**: Should check when dequeuing
3. **Modifying maze during pathfinding**: Keep visualization separate from algorithm
4. **Using wrong heuristic**: Manhattan for 4-directional, Euclidean/Chebyshev for 8-directional
5. **Integer overflow in position hashing**: Use proper hash function
6. **Not handling "no path" case**: Always check for empty result

---

## Additional Resources

- [Red Blob Games: A\* Pathfinding](https://www.redblobgames.com/pathfinding/a-star/introduction.html)
- [Wikipedia: Maze Generation Algorithms](https://en.wikipedia.org/wiki/Maze_generation_algorithm)
- [SFML Documentation](https://www.sfml-dev.org/documentation/)

---
## Third Party Coding Resources
These can be used for reference and you can use snippets of the code.  You can not copy the full solution from any third party code and use it as your solution.

1. **mazegen (Header-Only)**

GitHub: https://github.com/aleksandrbazhin/mazegen
Features: Header-only C++ library with zero dependencies, uses STL heavily. Based on Bob Nystrom's room-and-maze approach. GitHub
SFML Demo: https://github.com/aleksandrbazhin/mazegen_sfml_example
Why it's good: Students just include one header file, configure room sizes, wiggle chance, dead-end removal, etc.

2. **fuyalasmit/Maze-Generator-and-Solver (Complete Reference Project)**

GitHub: https://github.com/fuyalasmit/Maze-Generator-and-Solver
Uses recursive backtracking algorithm to create complex and solvable mazes, and implements the A* algorithm to efficiently find the shortest path. Developed as part of a third-semester computer engineering course demonstrating OOP concepts. GitHub
Why it's good: Almost exactly matches the homework requirements - could serve as a reference architecture.

3. **RamezzE/MazeSolver**

GitHub: https://github.com/RamezzE/MazeSolver
Maze generation, solving & finding shortest paths algorithms with backtracking, written in C++ and SFML. GitHub

4. **fawzeus/maze-generator (Multiple Algorithms)**

GitHub: https://github.com/fawzeus/maze-generator
Four algorithms implemented: Depth-First Search (DFS), Hunt and Kill, Prim's Maze, and the Shift Origin algorithm. Each algorithm is visually demonstrated. GitHub