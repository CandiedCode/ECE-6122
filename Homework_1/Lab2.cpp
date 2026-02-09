#include <iostream>
#include <MazeGenerator.h>
#include <SFML/Graphics.hpp>

#define WIDTH 800
#define HEIGHT 600

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

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Maze Generator");
    Maze maze(50, 50);
    maze.generate();
    std::cout << "Start position: (" << maze.getStart().first << ", " << maze.getStart().second << ")" << std::endl;
    std::cout << "End position: (" << maze.getEnd().first << ", " << maze.getEnd().second << ")" << std::endl;

    // Start the game loop
    while (window.isOpen())
    {
        // Clear the window
        window.clear(sf::Color::White);

        // Draw the maze
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
