/*
Author: Jennifer Cwagenberg
Class: ECE6122
Last Date Modified: 2026-01-22
Description:  Problem 3: Number Classification with Control Structures 

*/

#include <iostream>
#include <cmath>

bool isPositiveNumber(int number) {
    return number > 0;
}

bool isNegativeNumber(int number) {
    return number < 0;
}

bool isZero(int number) {
    return number == 0;
}

/**
 * @brief Function to check if a number is even
 * @param number The integer to check
 * @return true if the number is even, false otherwise
*/
bool isEvenNumber(int number) {
    return number % 2 == 0;
}

/**
 * @brief Function to check if a number is prime
 * @param number The integer to check
 * @return true if the number is prime, false otherwise
*/
bool isPrime(int number) {
    // 0 and 1 are not prime numbers
    if (number <= 1) {
        return false; 
    }

    for (int i = 2; i * i <= number; i++) {
        if (number % i == 0) {
            return false;
        }
    }
    return true;
}

bool isComposite(int number) {
    // Composite numbers are greater than 1 and not prime
    if (number <= 1) {
        return false;
    }
    return !isPrime(number);
}

bool isPerfectSquare(int number) {
    // Negative numbers cannot be perfect squares
    if (isNegativeNumber(number)) {
        return false;
    }

    double square_root = std::sqrt(number);
    double int_part;
    double frac_part = std::modf(square_root, &int_part);

    if (frac_part != 0.0) {
        return false;
    }
    return true;
}

bool isFibonacci(int number) {
    // Fibonacci numbers are non-negative
    if (isNegativeNumber(number)) {
        return false;
    }

    if (number > 50) {
        return false;
    }

    int a = 0;
    int b = 1;

    while (a < number) {
        int temp = a;
        a = b;
        b = temp + b;
    }

    return a == number;
}

void classifyNumber(int n, bool& isPositive, bool& isEven,  
                    bool& prime, bool& perfectSquare, bool& fibonacci){
    isPositive = isPositiveNumber(n);
    isEven = isEvenNumber(n);
    prime = isPrime(n);
    perfectSquare = isPerfectSquare(n);
    fibonacci = isFibonacci(n);
}


int main() {
    
    int number;
    
    do {
        std::cout << "Enter an integer number (-9999 to quit): ";
        std::cin >> number;

        if (std::cin.fail()) {
            std::cout << "Exiting program." << std::endl;
            break;
        }

        bool isPositive, isEven, prime, perfectSquare, fibonacci;
        classifyNumber(number, isPositive, isEven, prime, perfectSquare, fibonacci);

        std::cout << "Analysis of " << number << ":\n";
        std::cout << "\tSign: " << (isPositive ? "Positive" : "Negative") << "\n";
        std::cout << "\tParity: " << (isEven ? "Even" : "Odd") << "\n";
        std::cout << "\tPrimality: " << (prime ? "Prime" : "Composite") << "\n";
        std::cout << "\tPerfect Square: " << (perfectSquare ? "Yes" : "No") << "\n";
        std::cout << "\tFibonacci (<=50): " << (fibonacci ? "Yes" : "No") << "\n";
        std::cout << "\tMagnitude: " << std::abs(number) << "\n";
        std::cout << std::endl;
        
    } while (number != -9999);
}