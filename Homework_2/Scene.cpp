/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-02-13
 * Description:  Homework 2: Ray Tracing Visualization with Multiple Rendering Modes
 *
 *
 * @file Scene.cpp
 * @brief Scene implementation file. Contains the Scene class with methods for creating and managing spheres and walls in the scene.
 */

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
    : windowWidth(windowWidth), windowHeight(windowHeight), numSpheres(numSpheres), numWalls(numWalls), spheres(numSpheres),
      walls(numWalls), wallRotationCache(numWalls), rng(std::random_device{}())
{
    createScene();
}

auto Scene::createSphere(double radius) -> sf::CircleShape
{
    sf::CircleShape sphere(static_cast<float>(radius));
    sphere.setFillColor(sf::Color::Transparent);
    sphere.setOutlineThickness(2.F);
    sphere.setOutlineColor(sf::Color::White);
    return sphere;
}

auto Scene::createSphere(double radius, sf::Color color) -> sf::CircleShape
{
    sf::CircleShape sphere = createSphere(radius);
    sphere.setOutlineColor(color);
    return sphere;
}

auto Scene::createWall(double width, double height) -> sf::RectangleShape
{
    return sf::RectangleShape(sf::Vector2f(static_cast<float>(width), static_cast<float>(height)));
}

auto Scene::createWall(double width, double height, sf::Color color) -> sf::RectangleShape
{
    sf::RectangleShape wall = createWall(width, height);
    wall.setFillColor(color);
    return wall;
}

auto Scene::createSpheres() -> void
{
    std::uniform_int_distribution<int> radiusDistribution(50, 149); // 50 to 150 (exclusive upper)
    std::uniform_int_distribution<int> positionXDistribution(0, windowWidth - 1);
    std::uniform_int_distribution<int> positionYDistribution(0, windowHeight - 1);

    for (int i = 0; i < numSpheres; ++i)
    {
        // Generate random radius between 50 and 150 (diameter max 300 pixels)
        double radius = radiusDistribution(rng);
        // Create sphere using helper method
        sf::CircleShape sphere = createSphere(radius, sf::Color::Red);
        // Set random position within window bounds
        int posX = positionXDistribution(rng);
        int posY = positionYDistribution(rng);
        posX = std::max(0, posX - static_cast<int>(2 * radius) + 10);
        posY = std::max(0, posY - static_cast<int>(2 * radius) + 10);
        sphere.setPosition(static_cast<float>(posX), static_cast<float>(posY));
        // Add sphere to the scene
        spheres.at(i) = sphere;
    }
}

auto Scene::createWalls() -> void
{
    std::uniform_int_distribution<int> orientationDist(0, 1);
    std::uniform_int_distribution<int> widthDist(100, windowWidth - 100);
    std::uniform_int_distribution<int> heightDist(100, windowHeight - 100);
    std::uniform_int_distribution<int> colorDist(0, 255);
    std::uniform_int_distribution<int> positionXDist(0, windowWidth - 1);
    std::uniform_int_distribution<int> positionYDist(0, windowHeight - 1);
    std::uniform_int_distribution<int> rotationDist(0, 359);

    for (std::size_t i = 0; i < static_cast<std::size_t>(numWalls); ++i)
    {
        // One dimension is small (max 5 pixels), other is larger
        // Randomly choose which dimension is small
        int orientation = orientationDist(rng);
        double width{};
        double height{};

        if (orientation == 0)
        {
            // Horizontal rectangle (wide, short)
            width = 200.0 + widthDist(rng);
            height = 5;
        }
        else
        {
            // Vertical rectangle (narrow, tall)
            width = 5;
            height = 200.0 + heightDist(rng);
        }

        // Random color with RGB components between 0 and 255
        sf::Color color(colorDist(rng), colorDist(rng), colorDist(rng));
        // Create wall using helper method
        sf::RectangleShape wall = Scene::createWall(width, height, color);

        // Set fully random position within window bounds
        wall.setPosition(static_cast<float>(positionXDist(rng)), static_cast<float>(positionYDist(rng)));

        // Apply random rotation for diagonal effect
        auto rotation = static_cast<float>(rotationDist(rng));
        wall.setRotation(rotation);

        // Compute all 4 world-space corners of the rotated wall
        sf::Vector2f pos = wall.getPosition();
        sf::Vector2f size = wall.getSize();
        auto rot_rad = wall.getRotation() * static_cast<float>(M_PI) / 180.0F;
        auto cos_r = std::cos(rot_rad);
        auto sin_r = std::sin(rot_rad);

        std::array<sf::Vector2f, 4> localCorners = {sf::Vector2f{0.F, 0.F}, {size.x, 0.F}, {size.x, size.y}, {0.F, size.y}};

        auto minX = std::numeric_limits<float>::max();
        auto maxX = -std::numeric_limits<float>::max();
        auto minY = std::numeric_limits<float>::max();
        auto maxY = -std::numeric_limits<float>::max();

        for (const auto &c : localCorners)
        {
            float wx = (c.x * cos_r) - (c.y * sin_r) + pos.x;
            float wy = (c.x * sin_r) + (c.y * cos_r) + pos.y;
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
        else if (maxX > static_cast<float>(windowWidth))
        {
            offsetX = static_cast<float>(windowWidth) - maxX;
        }
        if (minY < 0.F)
        {
            offsetY = -minY;
        }
        else if (maxY > static_cast<float>(windowHeight))
        {
            offsetY = static_cast<float>(windowHeight) - maxY;
        }

        wall.setPosition(pos.x + offsetX, pos.y + offsetY);

        // Add wall to the scene and cache its sin/cos values for fast ray intersection
        walls.at(i) = wall;
        wallRotationCache.at(i) = {cos_r, sin_r};
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

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
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
            closest.color = sphere.getOutlineColor();
        }
    }

    // Check intersection with all walls (using cached sin/cos for efficiency)
    for (size_t i = 0; i < walls.size(); ++i)
    {
        const auto &[cos_cached, sin_cached] = wallRotationCache.at(i);
        HitResult hit = Geometry::intersectRectangle(ray, walls.at(i), cos_cached, sin_cached);
        if (hit.hit && hit.distance < closest.distance)
        {
            closest = hit;
            closest.color = walls.at(i).getFillColor();
        }
    }

    return closest;
}
