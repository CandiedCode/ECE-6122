# Slab Method for Ray-Rectangle Intersection

## Overview

The algorithm checks if a ray intersects with an axis-aligned rectangle by testing two "slabs" (X and Y ranges):

```text
┌─────────────────────────────────────────────────────────────┐
│                   STEP 1: X-SLAB CHECK                      │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  Ray origin ●                                                │
│             │                                                 │
│             │ ray direction →                                │
│             ▼                                                 │
│                                 ┌──── x_max                  │
│         x_min ─────┐            │                            │
│                    │ RECTANGLE  │                            │
│              [─────┼────────────┤]                           │
│              │     │            │                            │
│              [─────┴────────────┘]                           │
│                    │            │                            │
│         Enter X at │   t1    t2 │ Exit X                    │
│              distance units along ray                        │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

## Step-by-Step Process

### 1️⃣ X-Slab Phase

Calculate when the ray enters and exits the X-range of the rectangle:

```text
t1 = (x_min - ray.origin.x) / ray.direction.x   ← when ray enters X range
t2 = (x_max - ray.origin.x) / ray.direction.x   ← when ray exits X range

If t1 > t2, swap them (handle rays going left)
Update: t_min = max(t_min, t1)
        t_max = min(t_max, t2)
```

**Visual representation:**

```text
X-Axis:
←──────•─────[════ x_min to x_max ════]──────→ ray direction
       origin    t1            t2
```

### 2️⃣ Y-Slab Phase

Same process for the Y-axis:

```text
t1 = (y_min - ray.origin.y) / ray.direction.y   ← when ray enters Y range
t2 = (y_max - ray.origin.y) / ray.direction.y   ← when ray exits Y range

Update: t_min = max(t_min, t1)
        t_max = min(t_max, t2)
```

**Visual representation:**

```text
Y-Axis:
↑
│     [════ y_min to y_max ════]
│     t1            t2
│•
└─────→ ray direction
origin
```

## Intersection Detection

### Valid Intersection Scenario

When the X and Y ranges overlap, there's a hit:

```text
X-Slab:  ─────[t_min_x────────t_max_x]──────
Y-Slab:  ─────[t_min_y────────t_max_y]──────
Overlap: ─────[t_min─────────t_max]──────  ✓ HIT!

Intersection point = ray.origin + ray.direction * t_min
distance = t_min
```

### No Intersection Scenario

When the X and Y ranges don't overlap:

```text
X-Slab:  ─────[t_min_x────────t_max_x]──────
Y-Slab:           ────[t_min_y────────t_max_y]──────
Overlap:              (no overlap)  ✗ MISS!
```

## Key Edge Cases

| Case | Condition | Handling |
| ------ | ----------- | ---------- |
| **Ray parallel to X** | `abs(ray.direction.x) ≈ 0` | Check if ray's X is between x_min and x_max |
| **Ray parallel to Y** | `abs(ray.direction.y) ≈ 0` | Check if ray's Y is between y_min and y_max |
| **Ray behind rectangle** | `t_min < 0` | Return no hit (rectangle is behind ray origin) |
| **Invalid range** | `t_min > t_max` | Return no hit (ranges don't overlap) |

## Worked Example

### Setup

```text
Rectangle: position=(100, 50), size=(200, 100)
  → x_min=100, x_max=300, y_min=50, y_max=150

Ray: origin=(50, 100), direction=(1, 0)  [moving right →]
```

### X-Slab Calculation

```text
t1 = (100 - 50) / 1 = 50
t2 = (300 - 50) / 1 = 250

Ray enters X-range at t=50, exits at t=250
t_min = 50, t_max = 250
```

### Y-Slab Calculation

```text
Ray direction Y = 0 → ray is parallel to Y axis
Check: Is ray.origin.y (100) between y_min (50) and y_max (150)?
YES ✓ → ray crosses entire height of rectangle
```

### Result

```text
✓ HIT!
Closest intersection:
  distance = t_min = 50
  point = (50, 100) + (1, 0) * 50 = (100, 100)
```

## Code Implementation Overview

```cpp
HitResult Geometry::intersectRectangle(const Ray &ray, const sf::RectangleShape &wall)
{
    HitResult result;

    // Extract rectangle bounds
    float x_min = wall.getPosition().x;
    float x_max = x_min + wall.getSize().x;
    float y_min = wall.getPosition().y;
    float y_max = y_min + wall.getSize().y;

    // Initialize t range
    float t_min = 0.0f;
    float t_max = infinity;

    // X-slab intersection
    if (ray.direction.x != 0) {
        Calculate t1 and t2 for X boundaries
        Update t_min and t_max
    } else {
        Check if ray's X is within [x_min, x_max]
    }

    // Y-slab intersection
    if (ray.direction.y != 0) {
        Calculate t1 and t2 for Y boundaries
        Update t_min and t_max
    } else {
        Check if ray's Y is within [y_min, y_max]
    }

    // Check if valid intersection exists
    if (t_min >= 0 && t_min <= t_max) {
        result.hit = true;
        result.distance = t_min;
        result.point = ray.origin + ray.direction * t_min;
    }

    return result;
}
```

## Why This Algorithm Works

1. **Separating Axis Theorem**: A ray intersects an axis-aligned rectangle if and only if it intersects both the X-slab
    and Y-slab
2. **Efficient**: Only 4 divisions (or fewer with special cases), no trigonometry
3. **Robust**: Handles parallel rays, rays starting inside/outside the rectangle
4. **Ordered Intersection**: `t_min` automatically gives the closest intersection point

## Visualization Summary

```text
WITHOUT SLAB METHOD (4 edge tests needed):
       ┌─────────┐
       │ ╱test 1  │
       │╱         │
   ●──╱──test 2──┤
       │╲  test 3 │
       │ ╲────────┤
       │test 4    │
       └─────────┘

WITH SLAB METHOD (2 axis tests):
   X-test: ─────[████████]─────  ✓
   Y-test: ─────[████████]─────  ✓
   Result: ──[████]──  HIT at this overlap!
```
