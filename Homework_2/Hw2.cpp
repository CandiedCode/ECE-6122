#include "RayTracer.h"
#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

// @brief Enum to represent different rendering modes
enum class RenderMode
{
    SingleThreaded,
    OpenMP,
    StdThread
};

/** @brief Load a font from the specified file path
 * @param fontPath Path to the font file
 * @param executableDir Directory of the executable
 * @return Loaded sf::Font object
 */
sf::Font loadFont(const std::string &fontPath, const std::string &executableDir)
{
    sf::Font font;

    // Suppress SFML error output by redirecting stderr file descriptor
    // If the first load fails, SFML will print an error message to stderr.
    // We want to suppress this message since we will attempt a second load with a different path.
    // After the second attempt, we restore stderr to its original state.
    // Flush any pending output from stderr to ensure everything is written before we redirect
    fflush(stderr);

#ifdef _WIN32
    int saved_stderr = _dup(2);            // Duplicate file descriptor 2 (stderr) and returns a new file descriptor number
    int devnull = _open("NUL", _O_WRONLY); // Open the null device for writing
    _dup2(devnull, 2);                     // Redirect file descriptor 2 (stderr) to point to the null device
#else
    int saved_stderr = dup(2);                 // Duplicate file descriptor 2 (stderr) and returns a new file descriptor number
    int devnull = open("/dev/null", O_WRONLY); // Open the null device for writing
    dup2(devnull, 2);                          // Redirects file descriptor 2 (stderr) to point to the null device
#endif

    bool loaded = font.loadFromFile(fontPath);

    if (!loaded)
    {
        std::string fullPath = executableDir + "/" + fontPath;
        loaded = font.loadFromFile(fullPath);
    }

    // Restore stderr
    fflush(stderr);
#ifdef _WIN32
    _dup2(saved_stderr, 2);
    _close(devnull);
    _close(saved_stderr);
#else
    dup2(saved_stderr, 2);
    close(devnull);
    close(saved_stderr);
#endif

    if (!loaded)
    {
        std::cerr << "Failed to load font from " << fontPath << " or " << executableDir << "/" << fontPath << std::endl;
        exit(EXIT_FAILURE);
    }

    return font;
}

// @brief Convert RenderMode enum to string representation
std::string renderModeToString(RenderMode mode)
{
    switch (mode)
    {
    case RenderMode::SingleThreaded:
        return "Single-Threaded";
    case RenderMode::OpenMP:
        return "OpenMP";
    case RenderMode::StdThread:
        return "StdThread";
    default:
        return "Unknown";
    }
}

auto main(int argc, char *argv[]) -> int
{
    // Get the maximum number of threads available on the hardware
    int maxThreads = std::thread::hardware_concurrency();
    if (maxThreads == 0)
    {
        maxThreads = 1; // Fallback if hardware_concurrency() returns 0
    }
    std::cout << "Available CPU threads: " << maxThreads << "\n";
    int currentThreadCount = 2; // Default to 2 threads for parallel modes

    // Window and pane dimensions
    const int WINDOW_WIDTH = 1000;
    const int WINDOW_HEIGHT = 600;
    const int PANE_HEIGHT = 20;
    const int DRAWABLE_WIDTH = WINDOW_WIDTH;
    const int DRAWABLE_HEIGHT = WINDOW_HEIGHT - PANE_HEIGHT;

    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Ray Tracer");
    window.setFramerateLimit(60);
    RayTracer rayTracer;
    int numRays = 3600; // 3600; // 3600 rays for 1 degree resolution

    // Extract executable directory from argv[0]
    std::string executableDir = std::filesystem::path(argv[0]).parent_path().string();
    // Load font for text rendering
    sf::Font font = loadFont("fonts/KOMIKAP_.ttf", executableDir);

    // Create scene with adjusted drawable area (excluding pane)
    Scene scene(DRAWABLE_WIDTH, DRAWABLE_HEIGHT, 2, 4);
    std::vector<HitResult> results;

    RenderMode mode = RenderMode::SingleThreaded; // Change this to switch rendering mode

    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
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
                case sf::Keyboard::R:
                    scene.createScene(); // Regenerate scene with new random objects
                    std::cout << "Scene randomized\n";
                    break;
                case sf::Keyboard::W:
                case sf::Keyboard::Up:
                    currentThreadCount = std::min(maxThreads, currentThreadCount + 1);
                    std::cout << "Increased threads: " << currentThreadCount << "\n";
                    break;
                case sf::Keyboard::S:
                case sf::Keyboard::Down:
                    currentThreadCount = std::max(2, currentThreadCount - 1);
                    std::cout << "Decreased threads: " << currentThreadCount << "\n";
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
            rayTracer.castRaysOpenMP(mousePos, numRays, scene, results, currentThreadCount);
            break;
        case RenderMode::StdThread:
            rayTracer.castRaysStdThread(mousePos, numRays, scene, results, currentThreadCount);
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
            lines[(2 * i) + 1].color = sf::Color(255, 100, 0, 30); // dim at hit
        }

        // Clear window and draw scene
        window.clear(sf::Color::Black);
        window.draw(lines);
        scene.draw(window);

        // Draw status pane at bottom
        sf::RectangleShape pane(sf::Vector2f(DRAWABLE_WIDTH, PANE_HEIGHT));
        pane.setPosition(0, DRAWABLE_HEIGHT);
        pane.setFillColor(sf::Color(50, 50, 50, 200)); // Dark semi-transparent background

        window.draw(pane);

        // Draw RenderMode text on the left side of the pane
        sf::Text modeText(renderModeToString(mode), font, 12);
        modeText.setPosition(5, DRAWABLE_HEIGHT + 4);
        modeText.setFillColor(sf::Color::White);
        window.draw(modeText);

        window.display();
    }

    return 0;
}
