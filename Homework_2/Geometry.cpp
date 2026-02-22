#include "Geometry.h"

#include "RayTracer.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

constexpr float PARALLEL_THRESHOLD = std::numeric_limits<float>::epsilon() * 100.0F;

auto Geometry::intersectLineSegment(const Ray &ray, const sf::Vector2f &point1, const sf::Vector2f &point2) -> HitResult
{
    HitResult result;

    // Ray: R(t) = ray.origin + t * ray.direction  (t ≥ 0)
    // Segment: S(u) = p1 + u * (p2 - p1)          (0 ≤ u ≤ 1)

    sf::Vector2f segment_direction = point2 - point1;
    sf::Vector2f oc = point1 - ray.origin;

    // Calculate the 2D cross product (returns scalar)
    // A × B = A.x * B.y - A.y * B.x
    float denom = (ray.direction.x * segment_direction.y) - (ray.direction.y * segment_direction.x);

    // Check if ray and segment are parallel
    if (std::abs(denom) < PARALLEL_THRESHOLD)
    {
        // Ray and segment are parallel, no intersection
        return result;
    }

    // Solve for t and u using the parametric equations
    // t·d - u·s = oc
    // Using cross products:
    float t = (oc.x * segment_direction.y - oc.y * segment_direction.x) / denom;
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

auto Geometry::intersectRectangle(const Ray &ray, const sf::RectangleShape &wall) -> HitResult
{
    HitResult closest_result;
    closest_result.hit = false;
    closest_result.distance = std::numeric_limits<float>::max();

    // Get rectangle properties
    sf::Vector2f pos = wall.getPosition();
    sf::Vector2f size = wall.getSize();
    float rotation_degrees = wall.getRotation();
    float rotation_radians = rotation_degrees * static_cast<float>(M_PI) / 180.0F;

    // Calculate the four corners of the rectangle
    // Corners relative to position (0,0)
    std::vector<sf::Vector2f> corners = {// Top-left
                                         {0.0F, 0.0F},
                                         // Top-right
                                         {size.x, 0.0F},
                                         // Bottom-right
                                         {size.x, size.y},
                                         // Bottom-left
                                         {0.0F, size.y}};

    // Rotate and translate corners to world space
    float cos_rotate = std::cos(rotation_radians);
    float sin_rotate = std::sin(rotation_radians);

    std::vector<sf::Vector2f> rotated_corners;
    for (const auto &corner : corners)
    {
        // Rotate the corner around the origin
        float rotated_x = corner.x * cos_rotate - corner.y * sin_rotate;
        float rotated_y = corner.x * sin_rotate + corner.y * cos_rotate;

        // Translate to the rectangle's position
        rotated_corners.push_back({rotated_x + pos.x, rotated_y + pos.y});
    }

    // Test ray intersection against each edge of the rotated rectangle
    for (int i = 0; i < 4; ++i)
    {
        int next = (i + 1) % 4;
        HitResult edge_hit = intersectLineSegment(ray, rotated_corners[i], rotated_corners[next]);

        // Keep track of the closest valid hit
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

    // discriminant < 0 there is no intersection.
    if (discriminant < 0.0F)
    {
        return result;
    }

    // discriminant ≥ 0, then t = (−b ± √discriminant) / 2a
    float sqrt_discriminant = std::sqrt(discriminant);
    float t1 = (-b - sqrt_discriminant) / (2.0F * a);
    float t2 = (-b + sqrt_discriminant) / (2.0F * a);

    // take the smallest positive t
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

    // No valid intersection
    if (t < 0.0F)
    {
        return result;
    }

    result.hit = true;
    result.distance = t;
    result.point = ray.origin + (ray.direction * t);

    return result;
}
