# Maze Generation Algorithm Explanation
## Recursive Backtracking Visualization

### Key Concepts

The maze uses a **grid coordinate system** where:
- **Odd indices (1, 3, 5, 7...)** = Path cells (can be carved out)
- **Even indices (0, 2, 4, 6...)** = Wall cells (barriers between paths)

This ensures walls always exist between path cells, creating a proper maze structure.

---

## Step-by-Step Walkthrough

Let's trace through a small 7×7 maze starting at position (1,1):

### **STEP 1: Initialize Grid**
All cells start as walls:
```
█ █ █ █ █ █ █
█ · █ · █ · █
█ █ █ █ █ █ █
█ · █ · █ · █
█ █ █ █ █ █ █
█ · █ · █ · █
█ █ █ █ █ █ █
```
- `█` = Wall (even coordinates)
- `·` = Unvisited path cell (odd coordinates)

---

### **STEP 2: Start at (1,1), Mark as Path**
```
█ █ █ █ █ █ █
█ ✓ █ · █ · █
█ █ █ █ █ █ █
█ · █ · █ · █
█ █ █ █ █ █ █
█ · █ · █ · █
█ █ █ █ █ █ █
```
- `✓` = Current cell (visited, marked as Path)
- Call `carvePassages(1, 1)`

---

### **STEP 3: Get Unvisited Neighbors**
From position (1,1), look 2 cells away in 4 directions:
```
        ↑ (1-2=-1, invalid)

← (-1)  (1,1)  → (1+2=3)

        ↓ (1+2=3)
```

Valid unvisited neighbors at position (1,1):
- **(1, 3)** - Right neighbor ✓
- **(3, 1)** - Down neighbor ✓

Represented on grid with arrows:
```
█ █ █ █ █ █ █
█ ✓ █→· █ · █
█ █ █ █ █ █ █
█↓█ · █ · █
█ █ █ █ █ █ █
█ · █ · █ · █
█ █ █ █ █ █ █
```

---

### **STEP 4: Shuffle Neighbors & Choose First**
After `std::shuffle()`, let's say we randomly pick **(3, 1)** - the down neighbor:

```
█ █ █ █ █ █ █
█ ✓ █ · █ · █
█ █ █ █ █ █ █
█→✓ █ · █ · █  ← Current choice!
█ █ █ █ █ █ █
█ · █ · █ · █
█ █ █ █ █ █ █
```

---

### **STEP 5: Carve Wall Between Current and Neighbor**
Wall position = midpoint between (1,1) and (3,1):
- `wallRow = 1 + (3-1)/2 = 2`
- `wallCol = 1 + (1-1)/2 = 1`

Carve at position **(2,1)**:
```
█ █ █ █ █ █ █
█ ✓ █ · █ · █
█ ▫ █ █ █ █ █   ← Wall carved (2,1)
█ ✓ █ · █ · █
█ █ █ █ █ █ █
█ · █ · █ · █
█ █ █ █ █ █ █
```
- `▫` = Carve path through wall

---

### **STEP 6: Recurse - Move to (3,1)**
Now call `carvePassages(3, 1)`:

```
█ █ █ █ █ █ █
█ V █ · █ · █    (V = previously visited)
█ ▫ █ █ █ █ █
█ ✓ █ · █ · █    ← Current position now
█ █ █ █ █ █ █
█ · █ · █ · █
█ █ █ █ █ █ █
```

Get unvisited neighbors of (3,1):
- **(3, 3)** - Right ✓
- **(5, 1)** - Down ✓

---

### **STEP 7: Continue Pattern**
Let's say we pick **(3, 3)** - right neighbor:

Carve wall at **(3, 2)**:
```
█ █ █ █ █ █ █
█ V █ · █ · █
█ ▫ █ █ █ █ █
█ V ▫ ✓ █ · █    ← Path connects (3,1)→(3,3)
█ █ █ █ █ █ █
█ · █ · █ · █
█ █ █ █ █ █ █
```

---

### **STEP 8: Dead End - Backtrack**
Eventually at some cell, all neighbors are already visited (dead end).

The recursion **naturally backtracks** because:
1. `carvePassages(3, 3)` finishes its recursive calls
2. Returns to `carvePassages(3, 1)`
3. Continues with next shuffled neighbor or returns higher
4. This process continues until stack unwinds

```
█ █ █ █ █ █ █
█ V █ V █ · █    ← Explored, all neighbors visited
█ ▫ █ ▫ █ █ █
█ V ▫ V █ · █    ← Explored, backtrack now
█ █ █ █ █ █ █
█ · █ · █ · █    ← Unexplored area
█ █ █ █ █ █ █
```

Continue recursion unwinds and picks up remaining unvisited cells...

---

## Key Algorithm Points

### **Why 2-Cell Spacing?**
```
If neighbors were 1 cell away:     If neighbors are 2 cells away:
█ █ █ █                           █ █ █ █ █ █
█ ✓ ✓ █  (Problem: no walls!)     █ ✓ █ ✓ █ ✓
█ █ █ █                           █ █ █ █ █ █
                                  ✓ █ ✓ █ ✓
                                  █ █ █ █ █ █
```
2-cell spacing ensures there's always a wall between path cells.

### **Wall Carving Formula**
For moving from **(r1, c1)** to **(r2, c2)**:
```
wallRow = r1 + (r2 - r1) / 2
wallCol = c1 + (c2 - c1) / 2
```
This finds the wall cell exactly between two path cells.

### **Recursive Backtracking Behavior**
- Explores as far as possible (depth-first)
- When stuck, automatically backtracks via recursion
- Randomization (`std::shuffle`) creates variety
- No need for explicit stack—call stack handles backtracking

---

## Algorithm Summary
```
carvePassages(row, col):
  1. Mark (row, col) as visited Path
  2. Get all unvisited neighbors 2 cells away
  3. Shuffle them randomly
  4. For each neighbor:
     a. If still unvisited:
        - Carve wall between current and neighbor
        - Recursively call carvePassages(neighbor)
  5. Return (implicit backtracking)
```

This creates a **perfect maze**: no loops, exactly one path between any two points.
