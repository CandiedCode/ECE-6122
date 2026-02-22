#include "Geometry.h"
#include "RayTracer.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

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
    float denom = ray.direction.x * segment_dir.y - ray.direction.y * segment_dir.x;

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
        result.point = ray.origin + ray.direction * t;
    }

    return result;
}

HitResult Geometry::intersectRectangle(const Ray &ray, const sf::RectangleShape &wall)
{
    HitResult result;

    // Get rectangle bounds
    sf::Vector2f pos = wall.getPosition();
    sf::Vector2f size = wall.getSize();

    float x_min = pos.x;
    float x_max = pos.x + size.x;
    float y_min = pos.y;
    float y_max = pos.y + size.y;

    // find intersection times for x and y walls of the rectangle
    float t_min = 0.0f;
    float t_max = std::numeric_limits<float>::max();

    // Check x axis
    // Check if ray is parallel to x axis
    if (std::abs(ray.direction.x) > PARALLEL_THRESHOLD)
    {
        float t1 = (x_min - ray.origin.x) / ray.direction.x;
        float t2 = (x_max - ray.origin.x) / ray.direction.x;

        if (t1 > t2)
            std::swap(t1, t2);

        t_min = std::max(t_min, t1);
        t_max = std::min(t_max, t2);
    }
    else
    {
        // Ray doesn't intersect x range as it's parallel
        if (ray.origin.x < x_min || ray.origin.x > x_max)
            return result; // Ray doesn't intersect x range
    }

    // Check y axis
    // Check if ray is parallel to y axis
    if (std::abs(ray.direction.y) > PARALLEL_THRESHOLD)
    {
        float t1 = (y_min - ray.origin.y) / ray.direction.y;
        float t2 = (y_max - ray.origin.y) / ray.direction.y;
        if (t1 > t2)
            std::swap(t1, t2);
        t_min = std::max(t_min, t1);
        t_max = std::min(t_max, t2);
    }
    else
    {
        // Ray doesn't intersect y range as it's parallel
        if (ray.origin.y < y_min || ray.origin.y > y_max)
            return result; // Ray doesn't intersect y range
    }

    // Check if there's a valid intersection
    if (t_min < 0.0f || t_min > t_max)
        return result; // No intersection in front of ray

    result.hit = true;
    result.distance = t_min;
    result.point = ray.origin + ray.direction * t_min;

    return result;
}

HitResult Geometry::intersectCircle(const Ray &ray, const sf::CircleShape &circle)
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
    float a = 1.0f; // ray.direction · ray.direction
    float b = 2.0f * (oc.x * ray.direction.x + oc.y * ray.direction.y);
    float c = (oc.x * oc.x + oc.y * oc.y) - (radius * radius);

    // Discriminant
    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f)
    {
        return result; // No intersection
    }

    // Find the two solutions
    float sqrt_discriminant = std::sqrt(discriminant);
    float t1 = (-b - sqrt_discriminant) / (2.0f * a);
    float t2 = (-b + sqrt_discriminant) / (2.0f * a);

    // We want the closest intersection in front of the ray (t > 0)
    float t = -1.0f;
    if (t1 > 0.0f)
        t = t1;
    else if (t2 > 0.0f)
        t = t2;

    if (t < 0.0f)
        return result; // No valid intersection

    result.hit = true;
    result.distance = t;
    result.point = ray.origin + ray.direction * t;

    return result;
}
