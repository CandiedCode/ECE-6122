#include "Geometry.h"

#include "RayTracer.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <limits>
#include <math.h>
#include <utility>
#include <vector>

constexpr float PARALLEL_THRESHOLD = std::numeric_limits<float>::epsilon() * 100.0F;

HitResult Geometry::intersectLineSegment(const Ray &ray, const sf::Vector2f &p1, const sf::Vector2f &p2) // static in header
{
    HitResult result;

    // Ray: R(t) = ray.origin + t * ray.direction  (t ≥ 0)
    // Segment: S(u) = p1 + u * (p2 - p1)          (0 ≤ u ≤ 1)

    sf::Vector2f segment_dir = p2 - p1;
    sf::Vector2f oc = p1 - ray.origin;

    // Calculate the 2D cross product (returns scalar)
    // A × B = A.x * B.y - A.y * B.x
    float denom = (ray.direction.x * segment_dir.y) - (ray.direction.y * segment_dir.x);

    // Check if ray and segment are parallel
    if (std::abs(denom) < PARALLEL_THRESHOLD)
    {
        return result; // Ray and segment are parallel, no intersection
    }

    // Solve for t and u using the parametric equations
    // t·d - u·s = oc
    // Using cross products:
    float t = (oc.x * segment_dir.y - oc.y * segment_dir.x) / denom;
    float u = (oc.x * ray.direction.y - oc.y * ray.direction.x) / denom;

    // Valid intersection when:
    // - t ≥ 0 (intersection is in front of ray)
    // - 0 ≤ u ≤ 1 (intersection is on the segment)
    if (t >= 0.0F && u >= 0.0F && u <= 1.0F)
    {
        result.hit = true;
        result.distance = t;
        result.point = ray.origin + (ray.direction * t);
    }

    return result;
}

HitResult Geometry::intersectRectangle(const Ray &ray, const sf::RectangleShape &wall)
{
    HitResult closest_result;
    closest_result.hit = false;
    closest_result.distance = std::numeric_limits<float>::max();

    // Get rectangle properties
    sf::Vector2f pos = wall.getPosition();
    sf::Vector2f size = wall.getSize();
    float rotation_degrees = wall.getRotation();
    float rotation_radians = rotation_degrees * 3.14159265F / 180.0F;

    // Calculate the four corners of the unrotated rectangle
    // Corners relative to position (0,0)
    std::vector<sf::Vector2f> corners = {{0.0F, 0.0F}, {size.x, 0.0F}, {size.x, size.y}, {0.0F, size.y}};

    // Rotate and translate corners to world space
    float cos_rot = std::cos(rotation_radians);
    float sin_rot = std::sin(rotation_radians);

    std::vector<sf::Vector2f> rotated_corners;
    for (const auto &corner : corners)
    {
        // Rotate the corner around the origin
        float rotated_x = corner.x * cos_rot - corner.y * sin_rot;
        float rotated_y = corner.x * sin_rot + corner.y * cos_rot;

        // Translate to the rectangle's position
        rotated_corners.push_back({rotated_x + pos.x, rotated_y + pos.y});
    }

    // Test ray intersection against each edge of the rotated rectangle
    for (int i = 0; i < 4; ++i)
    {
        int next = (i + 1) % 4;
        HitResult edge_hit = intersectLineSegment(ray, rotated_corners[i], rotated_corners[next]);

        if (edge_hit.hit && edge_hit.distance < closest_result.distance)
        {
            closest_result = edge_hit;
        }
    }

    return closest_result;
}

auto Geometry::intersectCircle(const Ray &ray, const sf::CircleShape &circle) -> HitResult
{
    HitResult result;

    // Get circle center and radius
    // In SFML, CircleShape position is top-left, so center is position + radius
    float radius = circle.getRadius();
    sf::Vector2f center = circle.getPosition() + sf::Vector2f(radius, radius);

    // Vector from ray origin to circle center
    sf::Vector2f oc = ray.origin - center;

    // Quadratic formula: t²(d·d) + 2t(oc·d) + (oc·oc - r²) = 0
    // Since direction is normalized: d·d = 1
    float a = 1.0F; // ray.direction · ray.direction
    float b = 2.0F * (oc.x * ray.direction.x + oc.y * ray.direction.y);
    float c = (oc.x * oc.x + oc.y * oc.y) - (radius * radius);

    // Discriminant
    float discriminant = (b * b) - (4.0F * a * c);

    if (discriminant < 0.0F)
    {
        return result; // No intersection
    }

    // Find the two solutions
    float sqrt_discriminant = std::sqrt(discriminant);
    float t1 = (-b - sqrt_discriminant) / (2.0F * a);
    float t2 = (-b + sqrt_discriminant) / (2.0F * a);

    // We want the closest intersection in front of the ray (t > 0)
    float t = -1.0F;
    if (t1 > 0.0F)
    {
        t = t1;
    }
    else if (t2 > 0.0F)
    {
        t = t2;
    }

    if (t < 0.0F)
    {
        return result; // No valid intersection
    }

    result.hit = true;
    result.distance = t;
    result.point = ray.origin + (ray.direction * t);

    return result;
}
