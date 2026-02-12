/*
Author: Jennifer Cwagenberg
Class: ECE6122
Last Date Modified: 2026-01-22
Description:  Problem 3: Number Classification with Control Structures

Write a program that classifies integers based on multiple mathematical properties. For
each number entered: 1. Determine if it is: – Positive, negative, or zero – Even or odd – Prime or composite (for
positive integers > 1) – A perfect square – A Fibonacci number (within the first 50 Fibonacci numbers) 2. Implement the
following functions: bool isPrime(int n); bool isPerfectSquare(int n); bool isFibonacci(int n); void classifyNumber(int
n, bool& isPositive, bool& isEven, bool& prime, bool& perfectSquare, bool& fibonacci); 3. Use a while loop to
continuously prompt for numbers until the user enters a sentinel value (e.g., -9999) 4. Use a switch statement to
categorize numbers into ranges: – “Small” (1-100) – “Medium” (101-10000) – “Large” (10001-1000000) – “Very Large” (>
1000000) Requirements: - Use appropriate loop structures for primality testing - Combine multiple conditions using
logical operators (&&, ||, !) - Handle edge cases (0, 1, negative numbers) appropriately Sample Output: Enter an integer
(-9999 to quit): 28 Analysis of 28: Sign:           Positive Parity:         Even Primality:      Composite Perfect
Square: No Fibonacci:      No Magnitude:      Small

Enter an integer (-9999 to quit): 13
Analysis of 13:
  Sign:           Positive
  Parity:         Odd
  Primality:      Prime
  Perfect Square: No
  Fibonacci:      Yes
  Magnitude:      Small
*/

#include <cmath>
#include <iostream>
#include <limits>
#include <string>

/// Checks if a number is positive.
///
/// @param number The integer to check.
/// @return true if the number is greater than zero, false otherwise.
bool isPositiveNumber(int number)
{
    return number > 0;
}

/// Checks if a number is negative.
///
/// @param number The integer to check.
/// @return true if the number is less than zero, false otherwise.
bool isNegativeNumber(int number)
{
    return number < 0;
}

/// Checks if a number is zero.
///
/// @param number The integer to check.
/// @return true if the number equals zero, false otherwise.
bool isZero(int number)
{
    return number == 0;
}

/// Checks if a number is even.
///
/// @param number The integer to check.
/// @return true if the number is even, false otherwise.
bool isEvenNumber(int number)
{
    return number % 2 == 0;
}

/// Checks if a number is prime.
///
/// A prime number is a natural number greater than 1 that has no positive
/// divisors other than 1 and itself.
///
/// @param number The integer to check.
/// @return true if the number is prime, false otherwise.
bool isPrime(int number)
{
    // 0 and 1 are not prime numbers
    if (number <= 1)
    {
        return false;
    }

    for (int i = 2; i * i <= number; i++)
    {
        if (number % i == 0)
        {
            return false;
        }
    }
    return true;
}

/// Checks if a number is composite.
///
/// A composite number is a positive integer greater than 1 that is not prime.
///
/// @param number The integer to check.
/// @return true if the number is composite, false otherwise.
bool isComposite(int number)
{
    // Composite numbers are greater than 1 and not prime
    if (number <= 1)
    {
        return false;
    }
    return !isPrime(number);
}

/// Checks if a number is a perfect square.
///
/// A perfect square is a non-negative integer that is the square of an integer.
///
/// @param number The integer to check.
/// @return true if the number is a perfect square, false otherwise.
bool isPerfectSquare(int number)
{
    // Negative numbers cannot be perfect squares
    if (isNegativeNumber(number))
    {
        return false;
    }

    double square_root = std::sqrt(number);
    double int_part;
    double frac_part = std::modf(square_root, &int_part);

    if (frac_part != 0.0)
    {
        return false;
    }
    return true;
}

// Cache for Fibonacci numbers to avoid recomputation
int fib_cache[50] = {0, 1};

/// Checks if a number is a Fibonacci number within the first 50 numbers.
///
/// The Fibonacci sequence starts with 0 and 1, and each subsequent number
/// is the sum of the previous two. This function only checks for Fibonacci
/// numbers up to and including 50.
///
/// @param number The integer to check.
/// @return true if the number is a Fibonacci number (<=50), false otherwise.
bool isFibonacci(int number)
{
    // Fibonacci numbers are non-negative
    if (isNegativeNumber(number))
    {
        return false;
    }

    // Handle base cases
    if (number == 0 || number == 1)
    {
        return true;
    }


    for (int i = 2; i < 50; i++)
    {
        int current = fib_cache[i];

        // we haven't calculated this fib number yet
        if (current == 0)
        {
            current = fib_cache[i - 1] + fib_cache[i - 2];
            fib_cache[i] = current;
        }

        // number found in fib sequence
        if (current == number)
        {
            return true;
        }

        // exceeded number, not in fib sequence
        if (current > number)
        {
            return false;
        }
    }
    // number not found in first 50 fib numbers
    return false;
}

/// Classifies a number based on multiple mathematical properties.
///
/// This function analyzes an integer and determines whether it is positive,
/// even, prime, a perfect square, and/or a Fibonacci number.
///
/// @param n The integer to classify.
/// @param[out] isPositive Set to true if n is positive, false otherwise.
/// @param[out] isEven Set to true if n is even, false otherwise.
/// @param[out] prime Set to true if n is prime, false otherwise.
/// @param[out] perfectSquare Set to true if n is a perfect square, false otherwise.
/// @param[out] fibonacci Set to true if n is a Fibonacci number (<=50), false otherwise.
void classifyNumber(int n, bool &isPositive, bool &isEven, bool &prime, bool &perfectSquare, bool &fibonacci)
{
    isPositive = isPositiveNumber(n);
    isEven = isEvenNumber(n);
    prime = isPrime(n);
    perfectSquare = isPerfectSquare(n);
    fibonacci = isFibonacci(n);
}

/// Categorizes a number based on its magnitude.
///
/// This function uses a switch statement to classify integers into magnitude
/// ranges: Small (1-100), Medium (101-10000), Large (10001-1000000), Very Large
/// (>1000000), Zero (0), or Negative (<0).
///
/// @param n The integer to categorize.
/// @return A string representing the magnitude category.
std::string magnitude(int n)
{
    std::string category;
    switch (n)
    {
    case 1 ... 100:
        category = "Small";
        break;
    case 101 ... 10000:
        category = "Medium";
        break;
    case 10001 ... 1000000:
        category = "Large";
        break;
    default:
        if (n > 1000000)
        {
            category = "Very Large";
        }
        else if (n == 0)
        {
            category = "Zero";
        }
        else
        {
            category = "Negative";
        }
        break;
    }
    return category;
}

int main()
{
    int number;

    do
    {
        std::cout << "Enter an integer number (-9999 to quit): ";
        std::cin >> number;

        while (std::cin.fail())
        {
            std::cerr << "Error: Invalid input. Please enter a valid integer." << std::endl;
            std::cout << "Enter an integer number (-9999 to quit): ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin >> number;
        }
        if (number != -9999)
        {
            bool isPositive, isEven, prime, perfectSquare, fibonacci;
            classifyNumber(number, isPositive, isEven, prime, perfectSquare, fibonacci);

            std::cout << "Analysis of " << number << ":\n";
            std::cout << "  Sign: \t\t" << (number == 0 ? "Zero" : (isPositive ? "Positive" : "Negative")) << std::endl;
            std::cout << "  Parity: \t\t" << (isEven ? "Even" : "Odd") << std::endl;
            std::cout << "  Primality: \t\t"
                      << (number <= 1 ? "Neither Prime nor Composite" : (prime ? "Prime" : "Composite")) << std::endl;
            std::cout << "  Perfect Square: \t" << (perfectSquare ? "Yes" : "No") << std::endl;
            std::cout << "  Fibonacci: \t\t" << (fibonacci ? "Yes" : "No") << std::endl;
            std::cout << "  Magnitude: \t\t" << magnitude(number) << std::endl;
            std::cout << std::endl;
        }
    } while (number != -9999);
}

/*
Sample Output:

Enter an integer number (-9999 to quit): blarg
Error: Invalid input. Please enter a valid integer.
Enter an integer number (-9999 to quit): 28
Analysis of 28:
  Sign:                 Positive
  Parity:               Even
  Primality:            Composite
  Perfect Square:       No
  Fibonacci:            No
  Magnitude:            Small

Enter an integer number (-9999 to quit): 13
Analysis of 13:
  Sign:                 Positive
  Parity:               Odd
  Primality:            Prime
  Perfect Square:       No
  Fibonacci:            Yes
  Magnitude:            Small

Enter an integer number (-9999 to quit): 0
Analysis of 0:
  Sign:                 Zero
  Parity:               Even
  Primality:            Neither Prime nor Composite
  Perfect Square:       Yes
  Fibonacci:            Yes
  Magnitude:            Zero

Enter an integer number (-9999 to quit): 200
Analysis of 200:
  Sign:                 Positive
  Parity:               Even
  Primality:            Composite
  Perfect Square:       No
  Fibonacci:            No
  Magnitude:            Medium

Enter an integer number (-9999 to quit): 10001
Analysis of 10001:
  Sign:                 Positive
  Parity:               Odd
  Primality:            Composite
  Perfect Square:       No
  Fibonacci:            No
  Magnitude:            Large

Enter an integer number (-9999 to quit): 1000000
Analysis of 1000000:
  Sign:                 Positive
  Parity:               Even
  Primality:            Composite
  Perfect Square:       Yes
  Fibonacci:            No
  Magnitude:            Large

Enter an integer number (-9999 to quit): 1000001
Analysis of 1000001:
  Sign:                 Positive
  Parity:               Odd
  Primality:            Composite
  Perfect Square:       No
  Fibonacci:            No
  Magnitude:            Very Large

Enter an integer number (-9999 to quit): -55
Analysis of -55:
  Sign:                 Negative
  Parity:               Odd
  Primality:            Neither Prime nor Composite
  Perfect Square:       No
  Fibonacci:            No
  Magnitude:            Negative

Enter an integer number (-9999 to quit): -9999
*/
