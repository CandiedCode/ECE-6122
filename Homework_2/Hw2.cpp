/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-02-13
 * Description:  Homework 2: Ray Tracing Visualization with Multiple Rendering Modes
 *
 *
 * @file Hw2.cpp
 * @brief Main application for ray tracing visualization with multiple rendering modes
 */

#include "RayTracer.h"
#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

// @brief Enum to represent different rendering modes
enum class RenderMode : std::uint8_t
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

// @brief Convert string representation of render mode to RenderMode enum
auto renderModeFromString(const std::string &mode) -> RenderMode
{
    if (mode == "Single-Threaded")
        return RenderMode::SingleThreaded;
    else if (mode == "OpenMP")
        return RenderMode::OpenMP;
    else if (mode == "StdThread")
        return RenderMode::StdThread;
    else
        return RenderMode::SingleThreaded; // Default case
}

/** @brief Load a font from the specified file path
 * @param fontPath Path to the font file
 * @param executableDir Directory of the executable
 * @return Loaded sf::Font object
 */
auto loadFont(const std::string &fontPath, const std::string &executablePath) -> sf::Font
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
auto calculateThreads() -> int
{
    int maxThreads = static_cast<int>(std::thread::hardware_concurrency());
    if (maxThreads == 0)
    {
        maxThreads = 1; // Fallback if hardware_concurrency() returns 0
    }
    std::cout << "Available CPU threads: " << maxThreads << "\n";
    return maxThreads;
}

/** @brief Generate a VertexArray of lines representing rays from the light source to hit points
 *  @param numRays The number of rays cast
 *  @param mousePos The position of the light source (mouse cursor)
 *  @param results The vector of HitResult for each ray
 *  @return An sf::VertexArray containing line vertices for rendering rays
 */
auto getRays(int numRays, const sf::Vector2f &mousePos, const std::vector<HitResult> &results) -> sf::VertexArray
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

/** @brief Execute ray tracing based on the current rendering mode
 *  @param mode The current rendering mode (SingleThreaded, OpenMP, StdThread)
 *  @param rayTracer Reference to the RayTracer instance
 *  @param scene Reference to the Scene
 *  @param mousePos Position of the light source
 *  @param numRays Number of rays to cast
 *  @param currentThreadCount Number of threads to use for parallel modes
 *  @param results Vector to store ray intersection results
 *  @return Elapsed time in microseconds for the ray tracing operation
 */
auto executeRayTracing(RenderMode mode, RayTracer &rayTracer, const Scene &scene, const sf::Vector2f &mousePos, int numRays,
                       int currentThreadCount, std::vector<HitResult> &results) -> int32_t
{
    auto startTime = std::chrono::high_resolution_clock::now();

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

    auto stopTime = std::chrono::high_resolution_clock::now();
    return static_cast<int32_t>(std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime).count());
}

/** @brief Update timing history and calculate average when buffer is full
 *  @param timings Deque to store the last N timing measurements
 *  @param elapsed Elapsed time in microseconds for current iteration
 *  @param maxSize Maximum number of iterations to track
 *  @return Average time in microseconds if buffer is full, std::nullopt otherwise
 */
auto updateTimingAndGetAverage(std::deque<int32_t> &timings, int32_t elapsed, int maxSize) -> std::optional<int32_t>
{
    timings.push_back(elapsed);
    if (static_cast<int>(timings.size()) > maxSize)
    {
        timings.pop_front();
    }

    if (static_cast<int>(timings.size()) == maxSize)
    {
        int32_t sum = std::accumulate(timings.begin(), timings.end(), int32_t{0});
        return sum / maxSize;
    }

    return std::nullopt;
}

/**
 * @brief Parse command line arguments to configure rendering mode, number of rays, and number of threads
 * @param argc Argument count * @param argv Argument vector
 * @param mode Reference to string to store the rendering mode
 * @param numRays Reference to integer to store the number of rays
 * @param numThreads Reference to integer to store the number of threads
 */
void parseArgs(int argc, const char *argv[], RenderMode &mode, int &numThreads, int &numRays)
{
    // To process arguments, we expect two integers for height and width. If not provided, we use defaults.
    if (argc != 4 && argc > 1)
    {
        std::cout << "Usage: " << argv[0] << " <mode> <threads> <numRays> <numThreads>\n";
        return;
    }
    std::string modeStr = argv[1];
    mode = renderModeFromString(modeStr);
    numThreads = std::stoi(argv[2]);
    numRays = std::stoi(argv[3]);
}

auto main(int argc, const char *argv[]) -> int
{
    // Get the maximum number of threads available on the hardware
    const int maxThreads = calculateThreads();
    const int rayCountIncrement = 3600;

    // Track timing data for the last 60 iterations
    std::deque<int32_t> timings;
    const int MAX_ITERATIONS = 60;

    // Window and pane dimensions
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    const uint WINDOW_WIDTH = desktopMode.width > 0 ? desktopMode.width - 100 : 1000;   // Use desktop width or fallback to 1000
    const uint WINDOW_HEIGHT = desktopMode.height > 0 ? desktopMode.height - 100 : 600; // Use desktop height or fallback to 600
    const uint PANE_HEIGHT = 50;
    const uint DRAWABLE_WIDTH = WINDOW_WIDTH;
    const uint DRAWABLE_HEIGHT = WINDOW_HEIGHT - PANE_HEIGHT;

    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Ray Tracer");
    window.setFramerateLimit(60);

    RenderMode mode = RenderMode::SingleThreaded; // Default to single-threaded mode
    int numRays = rayCountIncrement;              // Default to 3600 rays for 1 degree resolution
    int currentThreadCount = 2;
    parseArgs(argc, argv, mode, currentThreadCount, numRays);

    RayTracer rayTracer; // Create an instance of the RayTracer class to perform ray tracing operations
    // Load font for text rendering
    sf::Font font = loadFont("fonts/KOMIKAP_.ttf", argv[0]);

    // Create scene with adjusted drawable area (excluding pane)
    Scene scene(DRAWABLE_WIDTH, DRAWABLE_HEIGHT, 2, 4);
    std::vector<HitResult> results;
    sf::RectangleShape pane(sf::Vector2f(DRAWABLE_WIDTH, PANE_HEIGHT));
    pane.setPosition(0, DRAWABLE_HEIGHT);
    pane.setFillColor(sf::Color(50, 50, 50, 200)); // Dark semi-transparent

    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                case sf::Keyboard::Q:
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
                    // Reset timing since we are switching modes
                    timings.clear();

                    // Cycle through render modes
                    if (mode == RenderMode::SingleThreaded)
                    {
                        mode = RenderMode::OpenMP;
                    }
                    else if (mode == RenderMode::OpenMP)
                    {
                        mode = RenderMode::StdThread;
                    }
                    else
                    {
                        mode = RenderMode::SingleThreaded;
                    }
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

        // Execute ray tracing and measure elapsed time
        int32_t elapsedMicroseconds = executeRayTracing(mode, rayTracer, scene, mousePos, numRays, currentThreadCount, results);
        sf::Text timingText;
        // Update timing history and get average if ready
        if (auto average = updateTimingAndGetAverage(timings, elapsedMicroseconds, MAX_ITERATIONS))
        {
            timingText.setString("Avg (" + std::to_string(MAX_ITERATIONS) + "): " + std::to_string(*average) + " microseconds");
            timingText.setFont(font);
            timingText.setCharacterSize(20);
            timingText.setFillColor(sf::Color::White);
        }

        // Create vertex array for rays based on hit results
        sf::VertexArray lines = getRays(numRays, mousePos, results);

        // Clear window and draw scene
        window.clear(sf::Color::Black);
        window.draw(lines);
        scene.draw(window);
        window.draw(pane);

        // Draw RenderMode text on the left side of the pane
        sf::Text modeText("Current Mode: " + renderModeToString(mode), font, 20);
        modeText.setPosition(5, DRAWABLE_HEIGHT + 4);
        modeText.setFillColor(sf::Color::White);

        sf::Text rayCount("Rays: " + std::to_string(numRays), font, 20);
        rayCount.setPosition(modeText.getPosition().x + modeText.getGlobalBounds().width + 25, DRAWABLE_HEIGHT + 4);
        rayCount.setFillColor(sf::Color::White);

        sf::Text threadCount("Threads: " + std::to_string(currentThreadCount), font, 20);
        threadCount.setPosition(rayCount.getPosition().x + rayCount.getGlobalBounds().width + 25, DRAWABLE_HEIGHT + 4);
        threadCount.setFillColor(sf::Color::White);

        timingText.setPosition(threadCount.getPosition().x + threadCount.getGlobalBounds().width + 25, DRAWABLE_HEIGHT + 4);
        window.draw(modeText);
        window.draw(timingText);
        window.draw(rayCount);
        window.draw(threadCount);

        // display window
        window.display();
    }

    return 0;
}
