#include <iostream>
#include <MazeGenerator.h>
#include <SFML/Graphics.hpp>

#define WIDTH 1000
#define HEIGHT 600
#define PANEL 300.f

struct MazeConfig
    {
        MazeConfig() : height(defaultSize), width(defaultSize) {}
        MazeConfig(int h, int w) : height(h), width(w) {}

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

int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Lab2: Maze Generator");
    window.setFramerateLimit(60);
    Maze maze(50, 50);
    maze.generate();

    // Panel title
    sf::RectangleShape panel({PANEL, static_cast<float>(HEIGHT)});
    panel.setPosition(WIDTH - PANEL, 0);
    panel.setFillColor(sf::Color::Red);
    panel.setOutlineThickness(1.f);
    panel.setOutlineColor(sf::Color::Black);

    sf::Text input;
    sf::Font font;
    if (!font.loadFromFile("/Users/jennifercwagenberg/Code/gaTech_v2/ECE-6122/Homework_1/KOMIKAP_.ttf"))
    {        
        std::cerr << "Failed to load font!" << std::endl;
        return EXIT_FAILURE;
    }
    input.setFont(font);
    input.setString("Maze Generator");
    input.setCharacterSize(24);
    input.setFillColor(sf::Color::Black);
    input.setPosition(WIDTH - PANEL + 10.f, 10.f);

    sf::Text mazWidth;
    mazWidth.setFont(font);
    mazWidth.setString("Maze Width:");
    mazWidth.setCharacterSize(18);
    mazWidth.setFillColor(sf::Color::Black);
    mazWidth.setPosition(WIDTH - PANEL + 10.f, 50.f);

    sf::Text mazHeight;
    mazHeight.setFont(font);
    mazHeight.setString("Maze Height:");
    mazHeight.setCharacterSize(18);
    mazHeight.setFillColor(sf::Color::Black);
    mazHeight.setPosition(WIDTH - PANEL + 10.f, 90.f);

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
        window.draw(input);
        window.draw(mazWidth);
        window.draw(mazHeight);
        maze.draw(window);

        // Display the rendered content
        window.display();
        
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();

            std::cout << "Event type: " << event.type << std::endl;
        }
    }

    // MazeConfig config;
    // int mHeight, mWidth;
    // CellType cellType = CellType::Wall;

    // std::cout << "Enter maze height: ";
    // std::cin >> config.height;
    
    // while (std::cin.fail() || !config.heightValid())
    // {        
    //     std::cin.clear(); // clear the error flag
    //     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard invalid input
    //     std::cout << "Invalid input. Please enter a positive integer for maze height: ";
    //     std::cin >> config.height;
    // }

    // std::cout << "Enter maze width: ";
    // std::cin >> config.width;
    // while (std::cin.fail() || !config.widthValid())
    // {        
    //     std::cin.clear(); // clear the error flag
    //     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard invalid input
    //     std::cout << "Invalid input. Please enter a positive integer for maze width: ";
    //     std::cin >> config.width;
    // }

    std::cout << "Hello, ECE-6122!" << std::endl;
    return EXIT_SUCCESS;
}
