#ifndef HOMEWORK_2_GEOMETRY_H_
#define HOMEWORK_2_GEOMETRY_H_

#include <SFML/Graphics.hpp>
#include <limits>

struct Ray
{
    sf::Vector2f origin{};
    sf::Vector2f direction{}; // unit vector
};

struct HitResult
{
    bool hit = false;
    float distance = std::numeric_limits<float>::max();
    sf::Vector2f point{};
};

class Geometry
{
  private:
  public:
    static HitResult intersectLineSegment(const Ray &ray, const sf::Vector2f &p1, const sf::Vector2f &p2);

    /** @brief Test intersection between a ray and an axis-aligned rectangle
     *  @param ray The ray to test
     *  @param rectangle The rectangle shape to test against
     *  @return HitResult containing intersection information
     */
    static HitResult intersectRectangle(const Ray &ray, const sf::RectangleShape &rectangle);

    /** @brief Test intersection between a ray and a circle (sphere in 2D)
     *  @param ray The ray to test
     *  @param circle The circle shape to test against
     *  @return HitResult containing intersection information
     */
    static HitResult intersectCircle(const Ray &ray, const sf::CircleShape &circle);

    HitResult closestIntersection(const Ray &ray) const;
};

#endif // HOMEWORK_2_GEOMETRY_H_
