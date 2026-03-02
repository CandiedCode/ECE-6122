/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-02-13
 * Description:  Homework 2: Ray Tracing Visualization with Multiple Rendering Modes
 *
 *
 * @file Geometry.h
 * @brief Geometry calculations header file for ray tracing visualization. Determines ray intersections with line segments, rectangles, and
 * circles.
 */

#ifndef HOMEWORK_2_GEOMETRY_H_
#define HOMEWORK_2_GEOMETRY_H_

#include <SFML/Graphics.hpp>
#include <limits>

// @brief Struct to represent a ray in 2D space
struct Ray
{
    sf::Vector2f origin;
    sf::Vector2f direction; // unit vector
};

// @brief Struct to represent the result of a ray intersection test
struct HitResult
{
    bool hit = false;
    float distance = std::numeric_limits<float>::max();
    sf::Vector2f point;
};

/**
 * @brief Geometry class containing static methods for ray-shape intersection tests
 * This class provides methods to test intersection between rays and various shapes (line segments, rectangles, circles).
 */
class Geometry
{
  private:
  public:
    /**
     * @brief Test intersection between a ray and a line segment defined by two points
     * @param ray The ray to test
     * @param point1 The first endpoint of the line segment
     * @param point2 The second endpoint of the line segment
     * @return HitResult containing intersection information
     */
    static auto intersectLineSegment(const Ray &ray, const sf::Vector2f &point1, const sf::Vector2f &point2) -> HitResult;

    /**
     * @brief Test intersection between a ray and a rectangle
     * @param ray The ray to test
     * @param rectangle The rectangle shape to test against
     * @return HitResult containing intersection information
     */
    static auto intersectRectangle(const Ray &ray, const sf::RectangleShape &rectangle) -> HitResult;

    /**
     * @brief Test intersection between a ray and a rectangle using cached sin/cos values
     * @param ray The ray to test
     * @param rectangle The rectangle shape to test against
     * @param cos_rotation Pre-computed cosine of rotation angle
     * @param sin_rotation Pre-computed sine of rotation angle
     * @return HitResult containing intersection information
     */
    static auto intersectRectangle(const Ray &ray, const sf::RectangleShape &rectangle, float cos_rotation, float sin_rotation)
        -> HitResult;

    /**
     * @brief Test intersection between a ray and a circle (sphere in 2D)
     * @param ray The ray to test
     * @param circle The circle shape to test against
     * @return HitResult containing intersection information
     */
    static auto intersectCircle(const Ray &ray, const sf::CircleShape &circle) -> HitResult;
};

#endif // HOMEWORK_2_GEOMETRY_H_
