#include <MazeGenerator.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

#define PANEL 250

struct MazeConfig
{
    MazeConfig()
    {
        // The maze must be at least 25x25 cells
        height = defaultSize;
        width = defaultSize;
    }
    MazeConfig(int h, int w) : height(h), width(w)
    {
        if (!heightValid() || !widthValid())
        {
            std::cerr << "Invalid maze dimensions. Height and width must be between 10 and 100." << std::endl;
            height = defaultSize;
            width = defaultSize;
            throw std::invalid_argument("Invalid maze dimensions. Height and width must be between 10 and 100.");
        }
    }

  public:
    int getHeight() const
    {
        return height;
    }
    int getWidth() const
    {
        return width;
    }

  private:
    int height;
    int width;
    int defaultSize = 25;

    bool heightValid()
    {
        return height >= 10 && height <= 100;
    }
    bool widthValid()
    {
        return width >= 10 && width <= 100;
    }
};

MazeConfig processArgs(int argc, char *argv[])
{
    MazeConfig config;

    std::cout << "Processing command line arguments..." << std::endl;
    std::cout << "Argument count: " << argc << std::endl;
    std::cout << "Arguments: ";
    for (int i = 0; i < argc; ++i)
    {
        std::cout << argv[i] << " ";
    }
    std::cout << std::endl;

    if (argc != 3)
    {
        std::cout << "Defaulting to maze size 25x25. To specify size, provide two arguments for height and width (e.g. "
                     "./maze 30 40)."
                  << std::endl;
        return MazeConfig();
    }

    int height = std::stoi(argv[1]);
    int width = std::stoi(argv[2]);
    return MazeConfig(height, width);
}

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

int main(int argc, char *argv[])
{
    // Get desktop resolution
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    int desktopWidth = desktopMode.width;
    int desktopHeight = desktopMode.height;

    MazeConfig config = processArgs(argc, argv);
    Maze maze(config.getHeight(), config.getWidth());
    int windowWidth, windowHeight;
    maze.getWindowSize(windowWidth, windowHeight, desktopWidth, desktopHeight);

    windowWidth += PANEL; // Add space for the panel

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Lab2: Maze Generator");
    window.setFramerateLimit(60);
    maze.generate();

    // Panel
    sf::RectangleShape panel({PANEL, static_cast<float>(windowHeight)});
    panel.setPosition(windowWidth - PANEL, 0);
    panel.setFillColor(sf::Color::Black);
    panel.setOutlineThickness(1.f);
    panel.setOutlineColor(sf::Color::Black);

    int panel_start = windowWidth - PANEL + 10.f; // Start of text in panel with some padding
    sf::Font font = loadFont("/Users/jennifercwagenberg/Code/gaTech_v2/ECE-6122/Homework_1/KOMIKAP_.ttf");

    // General information
    sf::Text pTitle = getText(font, "Maze Generator", 20, sf::Color::Yellow, panel_start, 10.f);
    sf::Text maxDimensions =
        getText(font, "Maze: " + std::to_string(maze.getHeight()) + "x" + std::to_string(maze.getWidth()), 16,
                sf::Color::White, panel_start, pTitle.getGlobalBounds().height + pTitle.getGlobalBounds().top + 10.f);
    sf::Text algorithm = getText(font, "Algorithm: DFS", 16, sf::Color::Blue, panel_start,
                                 maxDimensions.getGlobalBounds().height + maxDimensions.getGlobalBounds().top + 10.f);
    sf::Text diagonal = getText(font, "Diagonal: No", 16, sf::Color::White, panel_start,
                                algorithm.getGlobalBounds().height + algorithm.getGlobalBounds().top + 10.f);
    sf::Text animationSpeed = getText(font, "Speed: 100 STEPS/S", 16, sf::Color::White, panel_start,
                                      diagonal.getGlobalBounds().height + diagonal.getGlobalBounds().top + 10.f);

    // Statistics
    sf::Text statisticsTitle =
        getText(font, "-- Statistics --", 16, sf::Color::Green, panel_start,
                animationSpeed.getGlobalBounds().height + animationSpeed.getGlobalBounds().top + 20.f);
    sf::Text nodesExplored =
        getText(font, "Nodes Explored: 0", 16, sf::Color::White, panel_start,
                statisticsTitle.getGlobalBounds().height + statisticsTitle.getGlobalBounds().top + 10.f);
    sf::Text pathLength = getText(font, "Path Length: 0", 16, sf::Color::White, panel_start,
                                  nodesExplored.getGlobalBounds().height + nodesExplored.getGlobalBounds().top + 10.f);
    sf::Text timeTaken = getText(font, "Time: 0s", 16, sf::Color::White, panel_start,
                                 pathLength.getGlobalBounds().height + pathLength.getGlobalBounds().top + 10.f);
    sf::Text pathFound = getText(font, "Path Found: No", 16, sf::Color::Green, panel_start,
                                 timeTaken.getGlobalBounds().height + timeTaken.getGlobalBounds().top + 10.f);

    // Controls
    sf::Text controlsTitle = getText(font, "-- Controls --", 16, sf::Color::Green, panel_start,
                                     pathFound.getGlobalBounds().height + pathFound.getGlobalBounds().top + 20.f);
    sf::Text generate = getText(font, "G: Generate", 16, sf::Color::White, panel_start,
                                controlsTitle.getGlobalBounds().height + controlsTitle.getGlobalBounds().top + 10.f);
    sf::Text solve = getText(font, "S: Solve", 16, sf::Color::White, panel_start,
                             generate.getGlobalBounds().height + generate.getGlobalBounds().top + 10.f);
    sf::Text reset = getText(font, "R: Reset", 16, sf::Color::White, panel_start,
                             solve.getGlobalBounds().height + solve.getGlobalBounds().top + 10.f);
    sf::Text toggleAlgorithm = getText(font, "A: Toggle Algorithm", 16, sf::Color::White, panel_start,
                                       reset.getGlobalBounds().height + reset.getGlobalBounds().top + 10.f);
    sf::Text adjustSpeed =
        getText(font, "+/-: Adjust Speed", 16, sf::Color::White, panel_start,
                toggleAlgorithm.getGlobalBounds().height + toggleAlgorithm.getGlobalBounds().top + 10.f);
    sf::Text escape = getText(font, "ESC: Exit", 16, sf::Color::White, panel_start,
                              adjustSpeed.getGlobalBounds().height + adjustSpeed.getGlobalBounds().top + 10.f);

    // sf::Text mazWidth = getText(font, "Maze Width:", 18, sf::Color::Black, windowWidth - PANEL + 10.f, 50.f);
    // sf::Text mazHeight = getText(font, "Maze Height:", 18, sf::Color::Black, windowWidth - PANEL + 10.f, 90.f);
    sf::Clock clock;

    // Start the game loop
    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();

        // Clear the window
        window.clear(sf::Color::Black); // Dark background for contrast

        // Draw the maze
        maze.draw(window);
        window.draw(panel);
        window.draw(pTitle);
        window.draw(maxDimensions);
        window.draw(algorithm);
        window.draw(diagonal);
        window.draw(animationSpeed);
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
                case sf::Keyboard::G:
                    std::cout << "Generating new maze..." << std::endl;
                    maze.generate();
                    break;
                case sf::Keyboard::S:
                    std::cout << "Solving the maze..." << std::endl;
                    break;
                case sf::Keyboard::R:
                    std::cout << "Resetting the maze..." << std::endl;
                    break;
                case sf::Keyboard::Up:
                    std::cout << "Increasing maze height..." << std::endl;
                    break;
                case sf::Keyboard::Down:
                    std::cout << "Decreasing maze height..." << std::endl;
                    break;
                case sf::Keyboard::Right:
                    std::cout << "Increasing maze width..." << std::endl;
                    break;
                case sf::Keyboard::Left:
                    std::cout << "Decreasing maze width..." << std::endl;
                    break;
                // Add key didn't trigger on my mac thus added equal as plus is on the same key
                case sf::Keyboard::Equal:
                case sf::Keyboard::Add:
                    std::cout << "Increasing animation speed..." << std::endl;
                    break;
                // Added hyphen as minus is on the same key
                case sf::Keyboard::Hyphen:
                case sf::Keyboard::Subtract:
                    std::cout << "Decreasing animation speed..." << std::endl;
                    break;
                default:
                    std::cout << "Unhandled key press: " << event.key.code << std::endl;
                    break;
                }
            }

            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();

            // std::cout << "Event type: " << event.type << std::endl;
        }
    }

    std::cout << "Hello, ECE-6122!" << std::endl;
    return EXIT_SUCCESS;
}
