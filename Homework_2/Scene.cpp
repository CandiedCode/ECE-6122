#include "Scene.h"
#include "Geometry.h"
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <limits>
#include <random>

Scene::Scene(int windowWidth, int windowHeight, int numSpheres, int numWalls)
    : windowWidth(windowWidth), windowHeight(windowHeight), numSpheres(numSpheres), numWalls(numWalls)
{
    createScene();
}

sf::CircleShape Scene::createSphere(double radius)
{
    return sf::CircleShape(radius);
}

sf::CircleShape Scene::createSphere(double radius, sf::Color color)
{
    sf::CircleShape sphere = createSphere(radius);
    sphere.setFillColor(color);
    return sphere;
}

sf::RectangleShape Scene::createWall(double width, double height)
{
    return sf::RectangleShape(sf::Vector2f(width, height));
}

sf::RectangleShape Scene::createWall(double width, double height, sf::Color color)
{
    sf::RectangleShape wall = createWall(width, height);
    wall.setFillColor(color);
    return wall;
}

void Scene::createSpheres()
{
    // Reserve space for spheres to improve performance
    spheres.reserve(numSpheres);

    thread_local static unsigned int seed = std::random_device()();

    for (int i = 0; i < numSpheres; ++i)
    {
        // Generate random radius and color for the sphere
        // Random radius between 20 and 50
        double radius = 40.0 + (rand_r(&seed) % 200);
        // Random color with RGB components between 0 and 255
        sf::Color color(rand_r(&seed) % 256, rand_r(&seed) % 256, rand_r(&seed) % 256);
        // Create sphere using Geometry class
        // sf::CircleShape sphere = Geometry::createSphere(radius, color);
        // Set random position for the sphere
        // sphere.setPosition(static_cast<float>(rand() % 1000), static_cast<float>(rand() % 600));
        // Add sphere to the scene
        // spheres.push_back(sphere);
    }
}

void Scene::createWalls()
{
    // Reserve space for walls to improve performance
    walls.reserve(numWalls);

    thread_local static unsigned int seed = std::random_device()();

    for (int i = 0; i < numWalls; ++i)
    {
        // Generate random width, height, and color for the wall
        // Random width between 50 and 150
        double width = 50.0 + (rand_r(&seed) % 600);
        // Random height between 10 and 30
        double height = 5.0;
        // Random color with RGB components between 0 and 255
        sf::Color color(rand_r(&seed) % 256, rand_r(&seed) % 256, rand_r(&seed) % 256);
        // Create wall using Geometry class
        sf::RectangleShape wall = Scene::createWall(width, height, color);
        // Set random position for the wall
        wall.setPosition(static_cast<float>(rand_r(&seed) % 400), static_cast<float>(rand_r(&seed) % 400));
        // Add wall to the scene
        walls.push_back(wall);
    }
}

void Scene::createScene()
{
    createSpheres();
    createWalls();
}

void Scene::draw(sf::RenderWindow &window)
{
    // for (auto &sphere : spheres)
    // {
    //     window.draw(sphere);
    // }

    for (auto &wall : walls)
    {
        window.draw(wall);
    }
}

HitResult Scene::closestIntersection(const Ray &ray) const
{
    constexpr float MAX_RAY_DIST = 2000.0f;
    HitResult result;
    result.hit = false;
    result.distance = std::numeric_limits<float>::max();
    result.point = ray.origin + ray.direction * MAX_RAY_DIST; // Default far point if no hit

    // Check intersection with all spheres
    // for (const auto &sphere : spheres)
    // {
    //     HitResult hit = Geometry::intersectCircle(ray, sphere);
    //     if (hit.hit && hit.distance < closest.distance)
    //     {
    //         closest = hit;
    //     }
    // }

    // // Check intersection with all walls
    // for (const auto &wall : walls)
    // {
    //     HitResult hit = Geometry::intersectRectangle(ray, wall);
    //     if (hit.hit && hit.distance < closest.distance)
    //     {
    //         closest = hit;
    //     }
    // }

    return result;
}
