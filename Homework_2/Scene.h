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
    static sf::CircleShape createSphere(double radius);

    /** @brief Create a colored sphere (circle) with the specified radius and color
     *  @param radius The radius of the sphere
     *  @param color The fill color of the sphere
     *  @return An sf::CircleShape representing the colored sphere
     */
    static sf::CircleShape createSphere(double radius, sf::Color color);

    /** @brief Create a plane (rectangle) with the specified width and height
     *  @param width The width of the plane
     *  @param height The height of the plane
     *  @return An sf::RectangleShape representing the plane
     */
    static sf::RectangleShape createWall(double width, double height);

    /** @brief Create a colored plane (rectangle) with the specified dimensions and color
     *  @param width The width of the plane
     *  @param height The height of the plane
     *  @param color The fill color of the plane
     *  @return An sf::RectangleShape representing the colored plane
     */
    static sf::RectangleShape createWall(double width, double height, sf::Color color);

    /** @brief Create spheres in the scene
     */
    void createSpheres();
    void createWalls();

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
    void draw(sf::RenderWindow &window);

    /** @brief Find the closest intersection of a ray with scene geometry
     *  @param ray The ray to test
     *  @return HitResult containing the closest intersection, or no hit if nothing is intersected
     */
    HitResult closestIntersection(const Ray &ray) const;
    void createScene();
};

#endif // HOMEWORK_2_SCENE_H_
