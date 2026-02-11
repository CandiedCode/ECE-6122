#include <MazeGenerator.h>
#include <SFML/Graphics.hpp>
#include <iostream>

#define WIDTH 1000
#define HEIGHT 1000
#define PANEL 300.f

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
    MazeConfig config = processArgs(argc, argv);

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Lab2: Maze Generator");
    window.setFramerateLimit(60);
    Maze maze(config.getHeight(), config.getWidth());
    maze.generate();

    // Panel
    sf::RectangleShape panel({PANEL, static_cast<float>(HEIGHT)});
    panel.setPosition(WIDTH - PANEL, 0);
    panel.setFillColor(sf::Color::Red);
    panel.setOutlineThickness(1.f);
    panel.setOutlineColor(sf::Color::Black);

    sf::Font font = loadFont("/Users/jennifercwagenberg/Code/gaTech_v2/ECE-6122/Homework_1/KOMIKAP_.ttf");
    sf::Text pTitle = getText(font, "Maze Generator", 24, sf::Color::Black, WIDTH - PANEL + 10.f, 10.f);

    sf::Text mazWidth = getText(font, "Maze Width:", 18, sf::Color::Black, WIDTH - PANEL + 10.f, 50.f);
    sf::Text mazHeight = getText(font, "Maze Height:", 18, sf::Color::Black, WIDTH - PANEL + 10.f, 90.f);
    sf::Clock clock;

    // Start the game loop
    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        // sf::Vector2u windowSize = window.getSize();
        // std::cout << "Window size: " << windowSize.x << "x" << windowSize.y << std::endl;

        // Clear the window
        window.clear(sf::Color::White);

        // Draw the maze
        window.draw(panel);
        window.draw(pTitle);
        window.draw(mazWidth);
        window.draw(mazHeight);
        maze.draw(window);

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
