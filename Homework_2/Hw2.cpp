#include "RayTracer.h"
#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdio>
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

// @brief Convert RenderMode enum to string representation
auto renderModeToString(RenderMode mode) -> std::string
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

/** @brief Load a font from the specified file path
 * @param fontPath Path to the font file
 * @param executableDir Directory of the executable
 * @return Loaded sf::Font object
 */
sf::Font loadFont(const std::string &fontPath, const std::string &executablePath)
{
    // Extract executable directory from argv[0]
    std::string executableDir = std::filesystem::path(executablePath).parent_path().string();
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

// @brief Calculate the number of threads available on the hardware
int calculateThreads()
{
    int maxThreads = static_cast<int>(std::thread::hardware_concurrency());
    if (maxThreads == 0)
    {
        maxThreads = 1; // Fallback if hardware_concurrency() returns 0
    }
    std::cout << "Available CPU threads: " << maxThreads << "\n";
    return maxThreads;
}

sf::VertexArray getRays(int numRays, const sf::Vector2f &mousePos, const std::vector<HitResult> &results)
{
    sf::VertexArray lines(sf::Lines, 2 * numRays);
    for (int i = 0; i < numRays; ++i)
    {
        //  Starting Position (light source) is bright
        lines[2 * i].position = mousePos;
        lines[2 * i].color = sf::Color(255, 200, 50, 180); // bright at source

        // End point is dim if hit, or far if no hit
        lines[2 * i + 1].position = results[i].point;
        lines[(2 * i) + 1].color = sf::Color(255, 100, 0, 30); // dim at hit
    }

    return lines;
}

auto main(int argc, const char *argv[]) -> int
{
    // Get the maximum number of threads available on the hardware
    const int maxThreads = calculateThreads();
    int currentThreadCount = 2; // Default to 2 threads for parallel modes
    const int rayCountIncrement = 3600;

    // Window and pane dimensions
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    const uint WINDOW_WIDTH = desktopMode.width > 0 ? desktopMode.width - 100 : 1000;   // Use desktop width or fallback to 1000
    const uint WINDOW_HEIGHT = desktopMode.height > 0 ? desktopMode.height - 100 : 600; // Use desktop height or fallback to 600
    const uint PANE_HEIGHT = 50;
    const uint DRAWABLE_WIDTH = WINDOW_WIDTH;
    const uint DRAWABLE_HEIGHT = WINDOW_HEIGHT - PANE_HEIGHT;

    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Ray Tracer");
    window.setFramerateLimit(60);

    RayTracer rayTracer;             // Create an instance of the RayTracer class to perform ray tracing operations
    int numRays = rayCountIncrement; // 3600 rays for 1 degree resolution

    // Load font for text rendering
    sf::Font font = loadFont("fonts/KOMIKAP_.ttf", argv[0]);

    // Create scene with adjusted drawable area (excluding pane)
    Scene scene(DRAWABLE_WIDTH, DRAWABLE_HEIGHT, 2, 4);
    std::vector<HitResult> results;
    sf::RectangleShape pane(sf::Vector2f(DRAWABLE_WIDTH, PANE_HEIGHT));
    pane.setPosition(0, DRAWABLE_HEIGHT);
    pane.setFillColor(sf::Color(50, 50, 50, 200)); // Dark semi-transparent

    // Start in single-threaded mode
    RenderMode mode = RenderMode::SingleThreaded;

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
                    numRays += rayCountIncrement; // Increase rays by 3600 (1 degree resolution)
                    std::cout << "Increased rays: " << numRays << "\n";
                    break;
                case sf::Keyboard::Hyphen:
                case sf::Keyboard::Subtract:
                    // Decrease animation speed
                    numRays = std::max(rayCountIncrement, numRays - rayCountIncrement); // Decrease rays but not below 3600
                    std::cout << "Decreased rays: " << numRays << "\n";
                    break;
                case sf::Keyboard::M:
                    // Cycle through render modes
                    if (mode == RenderMode::SingleThreaded)
                        mode = RenderMode::OpenMP;
                    else if (mode == RenderMode::OpenMP)
                        mode = RenderMode::StdThread;
                    else
                        mode = RenderMode::SingleThreaded;
                    std::cout << "Switched to " << renderModeToString(mode) << " mode\n";
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

        // Create vertex array for rays based on hit results
        sf::VertexArray lines = getRays(numRays, mousePos, results);

        // Clear window and draw scene
        window.clear(sf::Color::Black);
        window.draw(lines);
        scene.draw(window);
        window.draw(pane);

        // Draw RenderMode text on the left side of the pane
        sf::Text modeText(renderModeToString(mode), font, 12);
        modeText.setPosition(5, DRAWABLE_HEIGHT + 4);
        modeText.setFillColor(sf::Color::White);
        window.draw(modeText);

        // display window
        window.display();
    }

    return 0;
}
