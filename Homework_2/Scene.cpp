#include "Scene.h"
#include "Geometry.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <random>

Scene::Scene(int windowWidth, int windowHeight, int numSpheres, int numWalls)
    : windowWidth(windowWidth), windowHeight(windowHeight), numSpheres(numSpheres), numWalls(numWalls)
{
    createScene();
}

auto Scene::createSphere(double radius) -> sf::CircleShape
{
    return sf::CircleShape(radius);
}

auto Scene::createSphere(double radius, sf::Color color) -> sf::CircleShape
{
    sf::CircleShape sphere = createSphere(radius);
    sphere.setFillColor(color);
    return sphere;
}

auto Scene::createWall(double width, double height) -> sf::RectangleShape
{
    return sf::RectangleShape(sf::Vector2f(width, height));
}

auto Scene::createWall(double width, double height, sf::Color color) -> sf::RectangleShape
{
    sf::RectangleShape wall = createWall(width, height);
    wall.setFillColor(color);
    return wall;
}

auto Scene::createSpheres() -> void
{
    // Reserve space for spheres to improve performance
    spheres.clear();
    spheres.reserve(numSpheres);

    thread_local static unsigned int seed = std::random_device()();

    for (int i = 0; i < numSpheres; ++i)
    {
        // Generate random radius between 10 and 50 (diameter max 100 pixels)
        double radius = 20.0 + (rand_r(&seed) % 100);
        // Random color with RGB components between 0 and 255
        // sf::Color color(rand_r(&seed) % 256, rand_r(&seed) % 256, rand_r(&seed) % 256);
        // Create sphere using helper method
        sf::CircleShape sphere = createSphere(radius, sf::Color::Blue);
        // Set random position within window bounds
        sphere.setPosition(static_cast<float>(rand_r(&seed) % (windowWidth - 10 - static_cast<int>(2 * radius))),
                           static_cast<float>(rand_r(&seed) % (windowHeight - 10 - static_cast<int>(2 * radius))));
        // Add sphere to the scene
        spheres.push_back(sphere);
    }
}

auto Scene::createWalls() -> void
{
    // Reserve space for walls to improve performance
    walls.clear();
    walls.reserve(numWalls);

    thread_local static unsigned int seed = std::random_device()();

    for (int i = 0; i < numWalls; ++i)
    {
        // One dimension is small (max 5 pixels), other is larger
        // Randomly choose which dimension is small
        int orientation = rand_r(&seed) % 2;
        double width{};
        double height{};

        if (orientation == 0)
        {
            // Horizontal rectangle (wide, short)
            width = std::min(200.0 + (rand_r(&seed) % (windowWidth - 100)), static_cast<double>(windowWidth - 100));
            height = 5;
        }
        else
        {
            // Vertical rectangle (narrow, tall)
            width = 5;
            height = std::min(200.0 + (rand_r(&seed) % (windowHeight - 100)), static_cast<double>(windowHeight - 100));
        }

        // Random color with RGB components between 0 and 255
        sf::Color color(rand_r(&seed) % 256, rand_r(&seed) % 256, rand_r(&seed) % 256);
        // Create wall using helper method
        sf::RectangleShape wall = Scene::createWall(width, height, color);

        // Set fully random position within window bounds
        wall.setPosition(static_cast<float>(rand_r(&seed) % windowWidth), static_cast<float>(rand_r(&seed) % windowHeight));

        // Apply random rotation for diagonal effect
        auto rotation = static_cast<float>(rand_r(&seed) % 360);
        wall.setRotation(rotation);

        // Compute all 4 world-space corners of the rotated wall
        sf::Vector2f pos = wall.getPosition();
        sf::Vector2f size = wall.getSize();
        float rot_rad = wall.getRotation() * 3.14159265F / 180.0F;
        float cos_r = std::cos(rot_rad);
        float sin_r = std::sin(rot_rad);

        std::array<sf::Vector2f, 4> localCorners = {sf::Vector2f{0.f, 0.f}, {size.x, 0.f}, {size.x, size.y}, {0.f, size.y}};

        float minX = std::numeric_limits<float>::max();
        float maxX = -std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();
        float maxY = -std::numeric_limits<float>::max();

        for (const auto &c : localCorners)
        {
            float wx = c.x * cos_r - c.y * sin_r + pos.x;
            float wy = c.x * sin_r + c.y * cos_r + pos.y;
            minX = std::min(minX, wx);
            maxX = std::max(maxX, wx);
            minY = std::min(minY, wy);
            maxY = std::max(maxY, wy);
        }

        // Compute and apply correction offset to shift the wall so all corners are within bounds
        float offsetX = 0.F;
        float offsetY = 0.F;

        if (minX < 0.F)
        {
            offsetX = -minX;
        }
        else if (maxX > windowWidth)
        {
            offsetX = static_cast<float>(windowWidth) - maxX;
        }
        if (minY < 0.F)
        {
            offsetY = -minY;
        }
        else if (maxY > windowHeight)
        {
            offsetY = static_cast<float>(windowHeight) - maxY;
        }

        wall.setPosition(pos.x + offsetX, pos.y + offsetY);

        // Add wall to the scene
        walls.push_back(wall);
    }
}

auto Scene::createScene() -> void
{
    createSpheres();
    createWalls();
}

auto Scene::draw(sf::RenderWindow &window) -> void
{
    for (auto &sphere : spheres)
    {
        window.draw(sphere);
    }

    for (auto &wall : walls)
    {
        window.draw(wall);
    }
}

auto Scene::closestIntersection(const Ray &ray) const -> HitResult
{
    constexpr float MAX_RAY_DIST = 2000.0F;
    HitResult closest;
    closest.hit = false;
    closest.distance = std::numeric_limits<float>::max();
    closest.point = ray.origin + (ray.direction * MAX_RAY_DIST); // Default far point if no hit

    // Check intersection with all spheres
    for (const auto &sphere : spheres)
    {
        HitResult hit = Geometry::intersectCircle(ray, sphere);
        if (hit.hit && hit.distance < closest.distance)
        {
            closest = hit;
        }
    }

    // Check intersection with all walls
    for (const auto &wall : walls)
    {
        HitResult hit = Geometry::intersectRectangle(ray, wall);
        if (hit.hit && hit.distance < closest.distance)
        {
            closest = hit;
        }
    }

    return closest;
}
