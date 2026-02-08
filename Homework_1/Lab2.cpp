#include <iostream>

struct MazeConfig
    {
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
    MazeConfig config;
    int mHeight, mWidth;

    std::cout << "Enter maze height: ";
    std::cin >> config.height;
    
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
    return 0;
}
