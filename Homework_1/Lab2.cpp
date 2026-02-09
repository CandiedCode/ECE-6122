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

sf::Font loadFont(const std::string& fontPath) {
    sf::Font font;
    if (!font.loadFromFile(fontPath)) {
        std::cerr << "Failed to load font from " << fontPath << std::endl;
        exit(EXIT_FAILURE);
    }
    return font;
}

sf::Text getText(sf::Font& font, const std::string& str, unsigned int size, sf::Color color, float x, float y) {
    sf::Text text;
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}
    
int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Lab2: Maze Generator");
    window.setFramerateLimit(60);
    Maze maze(50, 50);
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
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();

            std::cout << "Event type: " << event.type << std::endl;
        }
    }


    std::cout << "Hello, ECE-6122!" << std::endl;
    return EXIT_SUCCESS;
}
