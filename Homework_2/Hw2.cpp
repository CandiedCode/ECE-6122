#include "RayTracer.h"
#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

enum class RenderMode
{
    SingleThreaded,
    OpenMP,
    StdThread
};

auto main() -> int
{
    sf::RenderWindow window(sf::VideoMode({1000, 600}), "Ray Tracer");
    window.setFramerateLimit(60);
    RayTracer rayTracer;
    int numRays = 360; // 3600; // 3600 rays for 1 degree resolution

    // Create scene
    // Scene scene(2, 4); // Example: 2 spheres and 4 planes
    Scene scene(window.getSize().x, window.getSize().y, 2, 4);
    std::vector<HitResult> results;

    RenderMode mode = RenderMode::SingleThreaded; // Change this to switch rendering mode

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                    window.close();
                    break;
                case sf::Keyboard::Equal:
                case sf::Keyboard::Add:
                    // Increase animation speed
                    numRays += 3600; // Increase rays by 3600 (1 degree resolution)
                    std::cout << "Increased rays: " << numRays << "\n";
                    break;
                case sf::Keyboard::Hyphen:
                case sf::Keyboard::Subtract:
                    // Decrease animation speed
                    numRays = std::max(3600, numRays - 3600); // Decrease rays but not below 3600
                    std::cout << "Decreased rays: " << numRays << "\n";
                    break;
                default:
                    break;
                }
            }
        }

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        switch (mode)
        {
        case RenderMode::SingleThreaded:
            rayTracer.castRaysSingleThreaded(mousePos, numRays, scene, results);
            break;
        case RenderMode::OpenMP:
            rayTracer.castRaysOpenMP(mousePos, numRays, scene, results, 4); // Example: use 4 threads
            break;
        case RenderMode::StdThread:
            rayTracer.castRaysStdThread(mousePos, numRays, scene, results, 4); // Example: use 4 threads
            break;
        }

        sf::VertexArray lines(sf::Lines, 2 * numRays);
        for (int i = 0; i < numRays; ++i)
        {
            // std::cout << "Ray " << i << ", Point=(" << results[i].point.x << ", " << results[i].point.y << ")\n";
            //  Starting Position (light source) is bright
            lines[2 * i].position = mousePos;
            lines[2 * i].color = sf::Color(255, 200, 50, 180); // bright at source

            // End point is dim if hit, or far if no hit
            lines[2 * i + 1].position = results[i].point;
            lines[2 * i + 1].color = sf::Color(255, 100, 0, 30); // dim at hit
        }

        // Clear window and draw scene
        window.clear(sf::Color::Black);
        window.draw(lines);
        scene.draw(window);
        window.display();
    }

    return 0;
}
