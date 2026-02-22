# Ray-Line Segment Intersection

## Overview

This function tests if a ray intersects with a line segment using parametric equations and 2D cross products.

```
Ray:     R(t) = O + t·D      (t ≥ 0)
Segment: S(u) = P1 + u·(P2 - P1)  (0 ≤ u ≤ 1)
```

## Parametric Equations Explained

### Ray
```
R(t) = ray.origin + t * ray.direction
```

- **t = 0**: You're at the ray's origin
- **t > 0**: You've moved forward along the direction
- **t < 0**: Behind the ray (invalid for rays)

### Line Segment
```
S(u) = P1 + u * (P2 - P1)
```

- **u = 0**: You're at point P1
- **u = 0.5**: You're at the midpoint between P1 and P2
- **u = 1**: You're at point P2
- **u < 0 or u > 1**: Off the segment (invalid)

## Visual Representation

```
Ray starting at origin, moving right:
     t=0      t=3      t=6
      │        │        │
      ●────────x────────●──────→
      origin            (intersection at t=6)

Segment from P1 to P2:
      u=0     u=0.5    u=1
      │        │        │
      P1───────+────────P2
             (midpoint)

If they intersect, we need t and u such that:
R(t) = S(u)
```

## Mathematical Solution

### Setting Up the Equation

When ray and segment intersect:
```
O + t·D = P1 + u·(P2 - P1)

Rearranging:
t·D - u·(P2 - P1) = P1 - O
```

Let:
- `d = D` (ray direction)
- `s = P2 - P1` (segment direction)
- `oc = P1 - O` (vector from ray origin to segment start)

So: `t·d - u·s = oc`

### Solving Using Cross Products

In 2D, the cross product gives a scalar:
```
A × B = A.x * B.y - A.y * B.x
```

Taking cross products of both sides:

**For t (cross with segment direction):**
```
t·(d × s) - u·(s × s) = oc × s
t·(d × s) = oc × s          [since s × s = 0]
t = (oc × s) / (d × s)
```

**For u (cross with ray direction):**
```
t·(d × d) - u·(s × d) = oc × d
-u·(s × d) = oc × d         [since d × d = 0]
u = (d × oc) / (d × s)
```

**Denominator (determinant):**
```
denom = d × s = d.x * s.y - d.y * s.x
```

If `denom ≈ 0`: Ray and segment are parallel → no intersection

## Implementation

```cpp
HitResult Geometry::intersectLineSegment(const Ray& ray,
                                        const sf::Vector2f& p1,
                                        const sf::Vector2f& p2)
{
    HitResult result;

    // Direction vectors
    sf::Vector2f segment_dir = p2 - p1;
    sf::Vector2f oc = p1 - ray.origin;

    // 2D cross product: d × s
    float denom = ray.direction.x * segment_dir.y
                - ray.direction.y * segment_dir.x;

    // Check for parallel ray and segment
    if (std::abs(denom) < PARALLEL_THRESHOLD)
        return result;  // Parallel → no intersection

    // Solve for parameters
    float t = (oc.x * segment_dir.y - oc.y * segment_dir.x) / denom;
    float u = (ray.direction.x * oc.y - ray.direction.y * oc.x) / denom;

    // Valid intersection: t ≥ 0 AND 0 ≤ u ≤ 1
    if (t >= 0.0f && u >= 0.0f && u <= 1.0f)
    {
        result.hit = true;
        result.distance = t;
        result.point = ray.origin + ray.direction * t;
    }

    return result;
}
```

## Validity Conditions

For a valid intersection, we need:

| Condition | Meaning | Why |
|-----------|---------|-----|
| `t ≥ 0` | Ray moving forward | Intersection must be in front of ray origin |
| `u ≥ 0` | After segment start | Intersection on or after P1 |
| `u ≤ 1` | Before segment end | Intersection on or before P2 |

## Examples

### Example 1: Intersection Found

```
Ray:     origin=(0, 0), direction=(1, 0)  [moving right]
Segment: P1=(2, -1), P2=(2, 1)           [vertical line at x=2]

segment_dir = (2, 1) - (2, -1) = (0, 2)
oc = (2, -1) - (0, 0) = (2, -1)

denom = 1 * 2 - 0 * 0 = 2  ✓ not parallel

t = (2 * 2 - (-1) * 0) / 2 = 4 / 2 = 2
u = (1 * (-1) - 0 * 2) / 2 = -1 / 2 = -0.5

Check: t=2 ≥ 0 ✓, u=-0.5 < 0 ✗  → NO INTERSECTION
(Ray passes below the segment)
```

### Example 2: Intersection Found

```
Ray:     origin=(0, 0), direction=(1, 1)  [moving northeast]
Segment: P1=(1, 0), P2=(1, 2)            [vertical line at x=1]

segment_dir = (1, 2) - (1, 0) = (0, 2)
oc = (1, 0) - (0, 0) = (1, 0)

denom = 1 * 2 - 1 * 0 = 2  ✓ not parallel

t = (1 * 2 - 0 * 0) / 2 = 2 / 2 = 1
u = (1 * 0 - 1 * 1) / 2 = -1 / 2 = -0.5

Check: t=1 ≥ 0 ✓, u=-0.5 < 0 ✗  → NO INTERSECTION
```

### Example 3: Intersection Found

```
Ray:     origin=(0, 1), direction=(1, 0)  [moving right]
Segment: P1=(2, 0), P2=(2, 2)            [vertical line at x=2]

segment_dir = (2, 2) - (2, 0) = (0, 2)
oc = (2, 0) - (0, 1) = (2, -1)

denom = 1 * 2 - 0 * 0 = 2  ✓ not parallel

t = (2 * 2 - (-1) * 0) / 2 = 4 / 2 = 2
u = (1 * (-1) - 0 * 2) / 2 = -1 / 2 = -0.5

Check: t=2 ≥ 0 ✓, u=-0.5 < 0 ✗  → NO INTERSECTION
```

Actually, let me recalculate Example 3:
```
Ray:     origin=(0, 1), direction=(1, 0)  [moving right along y=1]
Segment: P1=(2, 0), P2=(2, 2)            [vertical line at x=2, y∈[0,2]]

At t=2: point = (0, 1) + 2*(1, 0) = (2, 1)  ✓ This is on the segment!

segment_dir = (0, 2)
oc = (2, -1)

denom = 1 * 2 - 0 * 0 = 2

t = (2 * 2 - (-1) * 0) / 2 = 4 / 2 = 2  ✓
u = (1 * (-1) - 0 * 2) / 2 = -1 / 2 = -0.5  ✗

Hmm, this doesn't seem right. Let me verify with the segment equation:
S(u) = (2, 0) + u*(0, 2) = (2, 2u)
At u=0.5: (2, 1)  ✓ That's correct!

So u should be 0.5, not -0.5. Let me recalculate:

u = (d.x * oc.y - d.y * oc.x) / denom
u = (1 * (-1) - 0 * 2) / 2
u = -1 / 2 = -0.5

Wait, that's still wrong. Let me double-check the formula...

Actually, I think I had the formula backwards. Let me reconsider:

From: t·d - u·s = oc
Cross with d: t·(d × d) - u·(s × d) = oc × d
           0 - u·(s × d) = oc × d
           u = -(oc × d) / (s × d) = (d × oc) / (d × s)

Actually wait, the order matters:
(s × d) = s.x*d.y - s.y*d.x = 0*0 - 2*1 = -2
(d × s) = d.x*s.y - d.y*s.x = 1*2 - 0*0 = 2

So (s × d) = -(d × s)

u·(s × d) = oc × d
u·(-(d×s)) = oc × d
-u·(d×s) = oc × d
u = -oc × d / (d × s) = (d × oc) / (d × s)

Hmm wait, let me be more careful:
d × oc = d.x*oc.y - d.y*oc.x = 1*(-1) - 0*2 = -1
d × s = d.x*s.y - d.y*s.x = 1*2 - 0*0 = 2

u = (d × oc) / (d × s) = -1 / 2 = -0.5

This still gives -0.5, which is wrong!

Let me go back to basics. We want to solve:
O + t*D = P1 + u*(P2 - P1)

In component form:
O.x + t*D.x = P1.x + u*(P2.x - P1.x)
O.y + t*D.y = P1.y + u*(P2.y - P1.y)

Substituting values:
0 + t*1 = 2 + u*0   →   t = 2
1 + t*0 = 0 + u*2   →   1 = 2u   →   u = 0.5

So t=2, u=0.5 is correct. But my formula is giving u=-0.5. Let me check the formula again...

Oh wait, I think I made an error. Let me recalculate using the correct approach.

From my code:
```
float u = (ray.direction.x * oc.y - ray.direction.y * oc.x) / denom;
```

With:
- ray.direction = (1, 0)
- oc = (2, -1)
- denom = 2

u = (1 * (-1) - 0 * 2) / 2 = -1/2 = -0.5

But this is giving -0.5, not 0.5!

Hmm, let me verify the formula derivation again. Actually, I think there might be a sign issue in my formula. Let me recalculate from scratch...

Actually, you know what, maybe the formula in the code I wrote has a sign issue. Let me verify it differently. I should use the standard formula which is:

For line-line intersection where:
L1: P1 + t*d1
L2: P2 + s*d2

The solution is:
det = d1 × d2
t = (P2 - P1) × d2 / det
s = (P2 - P1) × d1 / det

In our case:
Ray: O + t*d
Segment: P1 + u*s

So:
det = d × s = denom
(P1 - O) × s = oc × s
t = (oc × s) / denom ✓ This matches!

(P1 - O) × d = oc × d
u = (oc × d) / denom

But in my code I have:
u = (ray.direction.x * oc.y - ray.direction.y * oc.x) / denom;

Which is:
u = (d × oc) / denom = -(oc × d) / denom

So the formula should be negated! Let me check if this is intentional...

Actually wait, let me just verify with the example:
oc × d = oc.x*d.y - oc.y*d.x = 2*0 - (-1)*1 = 0 + 1 = 1
d × oc = d.x*oc.y - d.y*oc.x = 1*(-1) - 0*2 = -1

So oc × d = 1 and d × oc = -1, so d × oc = -(oc × d) ✓

So u = (d × oc) / denom = -1 / 2 = -0.5

But we expect u = 0.5, so there's definitely a sign error!

The correct formula should be:
u = (oc × d) / denom = 1 / 2 = 0.5 ✓

So in the code, I should have:
u = (oc.x * ray.direction.y - oc.y * ray.direction.x) / denom;

Instead of:
u = (ray.direction.x * oc.y - ray.direction.y * oc.x) / denom;

Let me fix this!
