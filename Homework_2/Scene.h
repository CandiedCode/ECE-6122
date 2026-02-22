#ifndef HOMEWORK_2_SCENE_H_
#define HOMEWORK_2_SCENE_H_

#include "Geometry.h"
#include <SFML/Graphics.hpp>
#include <vector>

// Forward declaration
class RayTracer;

/** @class Scene
 *  @brief Manages geometric objects (spheres and planes) in a ray tracing scene
 */
class Scene
{
  private:
    // Scene properties
    int windowWidth;
    int windowHeight;
    int numSpheres;
    int numWalls;
    std::vector<sf::CircleShape> spheres;
    std::vector<sf::RectangleShape> walls;

    /** @brief Create a sphere (circle) with the specified radius
     *  @param radius The radius of the sphere
     *  @return An sf::CircleShape representing the sphere
     */
    static auto createSphere(double radius) -> sf::CircleShape;

    /** @brief Create a colored sphere (circle) with the specified radius and color
     *  @param radius The radius of the sphere
     *  @param color The fill color of the sphere
     *  @return An sf::CircleShape representing the colored sphere
     */
    static auto createSphere(double radius, sf::Color color) -> sf::CircleShape;

    /** @brief Create a plane (rectangle) with the specified width and height
     *  @param width The width of the plane
     *  @param height The height of the plane
     *  @return An sf::RectangleShape representing the plane
     */
    static auto createWall(double width, double height) -> sf::RectangleShape;

    /** @brief Create a colored plane (rectangle) with the specified dimensions and color
     *  @param width The width of the plane
     *  @param height The height of the plane
     *  @param color The fill color of the plane
     *  @return An sf::RectangleShape representing the colored plane
     */
    static auto createWall(double width, double height, sf::Color color) -> sf::RectangleShape;

    /** @brief Create spheres in the scene
     */
    auto createSpheres() -> void;
    auto createWalls() -> void;

  public:
    /** @brief Construct a scene with specified number of spheres and walls
     *  @param windowWidth The width of the window
     *  @param windowHeight The height of the window
     *  @param numSpheres Number of spheres to create
     *  @param numWalls Number of walls to create
     */
    Scene(int windowWidth, int windowHeight, int numSpheres, int numWalls);

    /** @brief Draw all scene objects to the render window
     *  @param window The render window to draw to
     */
    auto draw(sf::RenderWindow &window) -> void;

    /** @brief Find the closest intersection of a ray with scene geometry
     *  @param ray The ray to test
     *  @return HitResult containing the closest intersection, or no hit if nothing is intersected
     */
    auto closestIntersection(const Ray &ray) const -> HitResult;
    auto createScene() -> void;
};

#endif // HOMEWORK_2_SCENE_H_
