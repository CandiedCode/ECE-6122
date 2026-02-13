#include "MazeGenerator.h"
#include "MazeSolver.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <string>

#define PANEL 250
#define DEFAULT_MAZE_SIZE 25
#define MIN_MAZE_SIZE 10
#define MAX_MAZE_SIZE 100
#define DEFAULT_PADDING 10.f
#define DEFAULT_FONT_SIZE 16

// @brief Maze configuration and validate dimensions
struct MazeConfig
{
    // @brief Default constructor initializes maze size to 25x25
    MazeConfig() : height(DEFAULT_MAZE_SIZE), width(DEFAULT_MAZE_SIZE)
    {
    }
    // @brief Parameterized constructor with validation
    // @param h Maze height
    // @param w Maze width
    // @throws std::invalid_argument if dimensions are out of valid range
    MazeConfig(int h, int w) : height(h), width(w)
    {
        if (!isDimensionValid())
        {
            std::cerr << "Invalid maze dimensions. Height and width must be between " << MIN_MAZE_SIZE << " and " << MAX_MAZE_SIZE << "."
                      << std::endl;
            throw std::invalid_argument("Invalid maze dimensions. Height and width must be between " + std::to_string(MIN_MAZE_SIZE) +
                                        " and " + std::to_string(MAX_MAZE_SIZE) + ".");
        }
    }

  public:
    // @brief Get maze height
    // @return Maze height
    int getHeight() const
    {
        return height;
    }
    // @brief Get maze width
    // @return Maze width
    int getWidth() const
    {
        return width;
    }

  private:
    int height;
    int width;

    // @brief Validate maze dimensions
    // @return True if dimensions are valid, false otherwise
    bool isDimensionValid()
    {
        return height >= MIN_MAZE_SIZE && height <= MAX_MAZE_SIZE && width >= MIN_MAZE_SIZE && width <= MAX_MAZE_SIZE;
    }
};

// @brief Algorithm types for maze solving
enum class AlgorithmType
{
    BFS,
    AStar
};

// @brief Process command line arguments to configure maze dimensions
// @param argc Argument count
// @param argv Argument vector
// @return MazeConfig object with specified or default dimensions
MazeConfig processArgs(int argc, char *argv[])
{
    MazeConfig config;

    std::cout << "Processing command line arguments..." << std::endl;
    std::cout << "Argument count: " << argc << std::endl;
    std::cout << "Arguments: ";

    // To process arguments, we expect two integers for height and width. If not provided, we use defaults.
    if (argc != 3)
    {
        std::cout << "Defaulting to maze size 25x25. To specify size, provide two arguments for height and width (e.g. "
                     "./maze 30 40)."
                  << std::endl;
        return MazeConfig();
    }

    char *height = argv[1];
    char *width = argv[2];
    int parsedHeight, parsedWidth;
    try
    {
        parsedHeight = std::stoi(height);
        parsedWidth = std::stoi(width);
    }
    catch (const std::exception &e)
    {
        // throw error with details about the failure and the received arguments
        throw std::invalid_argument("Error parsing Maze dimensions: " + std::string(e.what()) +
                                    ".  Received height: " + std::string(height) + ", width: " + std::string(width));
    }

    return MazeConfig(parsedHeight, parsedWidth);
}

// @brief Load a font from the specified file path
// @param fontPath Path to the font file
// @return Loaded sf::Font object
sf::Font loadFont(const std::string &fontPath)
{
    sf::Font font;
    if (!font.loadFromFile(fontPath))
    {
        std::cerr << "Failed to load font from " << fontPath << std::endl;
        exit(EXIT_FAILURE);
    }
    return font;
}

// @brief Create an sf::Text object with specified properties
// @param font Font to use for the text
// @param str String content of the text
// @param size Character size of the text
// @param color Color of the text
// @param x X position of the text
// @param y Y position of the text
// @return Configured sf::Text object
sf::Text getText(sf::Font &font, const std::string &str, unsigned int size, sf::Color color, float x, float y)
{
    sf::Text text;
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}

// @brief Update the maze solver animation based on elapsed time
// @param deltaTime Time elapsed since last update
// @param solving Reference to the boolean indicating if the solver is currently animating
// @param solver Reference to the MazeSolver object to perform the next step
void update(sf::Time deltaTime, bool &solving, MazeSolver &solver, sf::Time &animationSpeed, int &nodesExploredCount)
{
    static sf::Time accumulated;
    accumulated += deltaTime;
    if (accumulated >= animationSpeed && solving)
    {
        accumulated = sf::Time::Zero;
        if (solver.step(nodesExploredCount))
        {
            solving = false;
        }
    }
}

// @brief Calculate the animation speed in steps per second and update the animation speed accordingly
// @param animationSpeed Reference to the sf::Time object representing the current animation speed
// @param stepsPerSecond Reference to the integer representing the current steps per second
// @param increase Boolean indicating whether to increase or decrease the speed
void calculateStepsPerSecond(sf::Time &animationSpeed, int &stepsPerSecond, bool increase)
{
    if (stepsPerSecond <= 1 && !increase)
    {
        std::cout << "Minimum speed reached. Cannot decrease further." << std::endl;
        return;
    }
    if (stepsPerSecond >= 1000 && increase)
    {
        std::cout << "Maximum speed reached. Cannot increase further." << std::endl;
        return;
    }
    if (increase)
    {
        stepsPerSecond += 50;
    }
    else
    {
        stepsPerSecond -= 50;
    }
    int milisecondsPerStep = 1000.f / stepsPerSecond;
    animationSpeed = sf::milliseconds(milisecondsPerStep);
    std::cout << "Updated animation speed: " << stepsPerSecond << " steps per second (" << milisecondsPerStep << " ms per step)."
              << std::endl;
}

void switchAlgorithm(AlgorithmType &currentAlgorithm, MazeSolver *&solver, Maze &maze, sf::Text &algorithm)
{
    delete solver; // Clean up existing solver
    if (currentAlgorithm == AlgorithmType::BFS)
    {
        currentAlgorithm = AlgorithmType::AStar;
        solver = new AStarSearch(maze);
    }
    else
    {
        currentAlgorithm = AlgorithmType::BFS;
        solver = new BreadthFirstSearch(maze);
    }

    algorithm.setString("Algorithm: " + std::string(currentAlgorithm == AlgorithmType::BFS ? "BFS" : "A*"));
}

// @brief Reset the window components to their default state when generating a new maze or resetting the current maze
// @param solved Reference to the boolean indicating if the maze is solved
// @param solving Reference to the boolean indicating if the solver is currently animating
// @param pathLengthCount Reference to the integer representing the current path length count
// @param nodesExploredCount Reference to the integer representing the current nodes explored count
// @param pathFound Reference to the sf::Text object displaying whether a path was found
// @param nodesExplored Reference to the sf::Text object displaying the number of nodes explored
// @param pathLength Reference to the sf::Text object displaying the path length
void resetWindowComponents(bool &solved, bool &solving, int &pathLengthCount, int &nodesExploredCount, sf::Text &pathFound,
                           sf::Text &nodesExplored, sf::Text &pathLength)
{
    solved = false;
    solving = false;
    pathLengthCount = 0;
    nodesExploredCount = 0;
    pathFound.setString("Path Found: No");
    nodesExplored.setString("Nodes Explored: 0");
    pathLength.setString("Path Length: 0");
}

int main(int argc, char *argv[])
{
    // Get desktop resolution
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    int desktopWidth = desktopMode.width;
    int desktopHeight = desktopMode.height;

    // Process command line arguments to configure maze dimensions
    MazeConfig config = processArgs(argc, argv);
    Maze maze(config.getHeight(), config.getWidth());

    // Calculate window size based on maze dimensions and cell size, ensuring it fits within the desktop resolution
    int windowWidth, windowHeight;
    maze.getWindowSize(windowWidth, windowHeight, desktopWidth, desktopHeight);

    // Add additional width for the information panel on the right
    windowWidth += PANEL; // Add space for the panel

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Lab2: Maze Generator");
    // Set FPS limit to make animation smoother and more consistent across different machines
    window.setFramerateLimit(60);
    maze.generate();
    AlgorithmType currentAlgorithm = AlgorithmType::BFS;
    MazeSolver *solver = new BreadthFirstSearch(maze);

    // Panel
    sf::RectangleShape panel({PANEL, static_cast<float>(windowHeight)});
    panel.setPosition(windowWidth - PANEL, 0);
    panel.setFillColor(sf::Color::Black);
    panel.setOutlineThickness(1.f);
    panel.setOutlineColor(sf::Color::Black);

    int panel_start = windowWidth - PANEL + DEFAULT_PADDING; // Start of text in panel with some padding
    sf::Font font = loadFont("fonts/KOMIKAP_.ttf");

    sf::Time animationSpeed = sf::milliseconds(10);
    int stepsPerSecond = 100;
    bool solved = false;
    int nodesExploredCount = 0;
    int pathLengthCount = 0;

    // General information
    sf::Text pTitle = getText(font, "Maze Generator", 20, sf::Color::Yellow, panel_start, DEFAULT_PADDING);
    sf::Text maxDimensions =
        getText(font, "Maze: " + std::to_string(maze.getHeight()) + " x " + std::to_string(maze.getWidth()), DEFAULT_FONT_SIZE,
                sf::Color::White, panel_start, pTitle.getGlobalBounds().height + pTitle.getGlobalBounds().top + (2 * DEFAULT_PADDING));
    sf::Text algorithm = getText(font, "Algorithm: " + std::string(currentAlgorithm == AlgorithmType::BFS ? "BFS" : "A*"),
                                 DEFAULT_FONT_SIZE, sf::Color(173, 216, 230), panel_start,
                                 maxDimensions.getGlobalBounds().height + maxDimensions.getGlobalBounds().top + DEFAULT_PADDING);
    sf::Text diagonal = getText(font, "Diagonal: No", DEFAULT_FONT_SIZE, sf::Color::White, panel_start,
                                algorithm.getGlobalBounds().height + algorithm.getGlobalBounds().top + DEFAULT_PADDING);
    sf::Text speed = getText(font, "Speed: " + std::to_string(stepsPerSecond) + " STEPS/S", DEFAULT_FONT_SIZE, sf::Color::White,
                             panel_start, diagonal.getGlobalBounds().height + diagonal.getGlobalBounds().top + DEFAULT_PADDING);

    // Statistics
    sf::Text statisticsTitle = getText(font, "-- Statistics --", DEFAULT_FONT_SIZE, sf::Color::Green, panel_start,
                                       speed.getGlobalBounds().height + speed.getGlobalBounds().top + (2 * DEFAULT_PADDING));
    sf::Text nodesExplored =
        getText(font, "Nodes Explored: " + std::to_string(nodesExploredCount), DEFAULT_FONT_SIZE, sf::Color::White, panel_start,
                statisticsTitle.getGlobalBounds().height + statisticsTitle.getGlobalBounds().top + DEFAULT_PADDING);
    sf::Text pathLength = getText(font, "Path Length: " + std::to_string(pathLengthCount), DEFAULT_FONT_SIZE, sf::Color::White, panel_start,
                                  nodesExplored.getGlobalBounds().height + nodesExplored.getGlobalBounds().top + DEFAULT_PADDING);
    sf::Text timeTaken = getText(font, "Time: 0s", DEFAULT_FONT_SIZE, sf::Color::White, panel_start,
                                 pathLength.getGlobalBounds().height + pathLength.getGlobalBounds().top + DEFAULT_PADDING);
    sf::Text pathFound = getText(font, "Path Found: " + std::string(solved ? "Yes" : "No"), DEFAULT_FONT_SIZE, sf::Color::Green,
                                 panel_start, timeTaken.getGlobalBounds().height + timeTaken.getGlobalBounds().top + DEFAULT_PADDING);

    // Controls
    sf::Text controlsTitle = getText(font, "-- Controls --", DEFAULT_FONT_SIZE, sf::Color::Green, panel_start,
                                     pathFound.getGlobalBounds().height + pathFound.getGlobalBounds().top + 20.f);
    sf::Text generate = getText(font, "G: Generate", DEFAULT_FONT_SIZE, sf::Color::White, panel_start,
                                controlsTitle.getGlobalBounds().height + controlsTitle.getGlobalBounds().top + DEFAULT_PADDING);
    sf::Text solve = getText(font, "S: Solve", DEFAULT_FONT_SIZE, sf::Color::White, panel_start,
                             generate.getGlobalBounds().height + generate.getGlobalBounds().top + DEFAULT_PADDING);
    sf::Text reset = getText(font, "R: Reset", DEFAULT_FONT_SIZE, sf::Color::White, panel_start,
                             solve.getGlobalBounds().height + solve.getGlobalBounds().top + DEFAULT_PADDING);
    sf::Text toggleAlgorithm = getText(font, "A: Toggle Algorithm", DEFAULT_FONT_SIZE, sf::Color::White, panel_start,
                                       reset.getGlobalBounds().height + reset.getGlobalBounds().top + DEFAULT_PADDING);
    sf::Text adjustSpeed = getText(font, "+/-: Adjust Speed", DEFAULT_FONT_SIZE, sf::Color::White, panel_start,
                                   toggleAlgorithm.getGlobalBounds().height + toggleAlgorithm.getGlobalBounds().top + DEFAULT_PADDING);
    sf::Text escape = getText(font, "ESC: Exit", DEFAULT_FONT_SIZE, sf::Color::White, panel_start,
                              adjustSpeed.getGlobalBounds().height + adjustSpeed.getGlobalBounds().top + DEFAULT_PADDING);

    sf::Clock clock;
    sf::Time elapsedTime;
    bool solving = false;

    // Start the game loop
    while (window.isOpen())
    {
        // Clear the window
        window.clear(sf::Color::Black); // Dark background for contrast

        // Draw all the things
        maze.draw(window);
        window.draw(panel);
        window.draw(pTitle);
        window.draw(maxDimensions);
        window.draw(algorithm);
        window.draw(diagonal);
        window.draw(speed);
        window.draw(statisticsTitle);
        window.draw(nodesExplored);
        window.draw(pathLength);
        window.draw(timeTaken);
        window.draw(pathFound);
        window.draw(controlsTitle);
        window.draw(generate);
        window.draw(solve);
        window.draw(reset);
        window.draw(toggleAlgorithm);
        window.draw(adjustSpeed);
        window.draw(escape);

        // Display the rendered content
        window.display();

        // if we are currently solving, update the animation based on elapsed time
        if (solving)
        {
            sf::Time deltaTime = clock.restart();
            update(deltaTime, solving, *solver, animationSpeed, nodesExploredCount);

            // If we just finished solving, reconstruct the path and update statistics
            if (!solving)
            {
                solved = true;
                pathFound.setString("Path Found: " + std::string(solved ? "Yes" : "No"));
                nodesExplored.setString("Nodes Explored: " + std::to_string(nodesExploredCount));
                std::list<Position> path = solver->reconstructPath();
                pathLengthCount = path.size();
                pathLength.setString("Path Length: " + std::to_string(pathLengthCount));
            }
        }
        else
        {
            // Reset clock when not solving to avoid large delta on next update
            clock.restart();
        }

        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                    window.close();
                    break;
                case sf::Keyboard::A:
                    std::cout << "Toggling algorithm..." << std::endl;
                    switchAlgorithm(currentAlgorithm, solver, maze, algorithm);
                    resetWindowComponents(solved, solving, pathLengthCount, nodesExploredCount, pathFound, nodesExplored, pathLength);
                    solver->reset();
                    maze.resetVisualization();
                    maze.draw(window);
                    break;
                case sf::Keyboard::G:
                    std::cout << "Generating new maze..." << std::endl;
                    maze.generate();
                    resetWindowComponents(solved, solving, pathLengthCount, nodesExploredCount, pathFound, nodesExplored, pathLength);
                    break;
                case sf::Keyboard::S:
                    std::cout << "Solving the maze..." << std::endl;
                    solving = true;
                    solver->reset();
                    break;
                case sf::Keyboard::R:
                    std::cout << "Resetting the maze..." << std::endl;
                    maze.resetVisualization();
                    maze.draw(window);
                    resetWindowComponents(solved, solving, pathLengthCount, nodesExploredCount, pathFound, nodesExplored, pathLength);
                    solver->reset();
                    break;
                // Add key didn't trigger on my mac thus added equal as plus is on the same key
                case sf::Keyboard::Equal:
                case sf::Keyboard::Add:
                    calculateStepsPerSecond(animationSpeed, stepsPerSecond, true);
                    speed.setString("Speed: " + std::to_string(stepsPerSecond) + " STEPS/S");
                    break;
                // Added hyphen as minus is on the same key
                case sf::Keyboard::Hyphen:
                case sf::Keyboard::Subtract:
                    std::cout << "Decreasing animation speed..." << std::endl;
                    calculateStepsPerSecond(animationSpeed, stepsPerSecond, false);
                    speed.setString("Speed: " + std::to_string(stepsPerSecond) + " STEPS/S");
                    break;
                default:
                    // std::cout << "Unhandled key press: " << event.key.code << std::endl;
                    break;
                }
            }

            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return EXIT_SUCCESS;
}
