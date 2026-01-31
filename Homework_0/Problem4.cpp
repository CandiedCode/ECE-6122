/*
Author: Jennifer Cwagenberg
Class: ECE6122
Last Date Modified: 2026-01-22
Description:  Problem 4: Generic Programming with Function Templates

Implement a set of function templates that work with multiple data types. This problem
emphasizes code reusability through generic programming.
Part A: Basic Function Templates (10 points)
Implement the following function templates:
// Returns the larger of two values
template <typename T>
T maximum(T a, T b);

// Returns the smaller of two values
template <typename T>
T minimum(T a, T b);

// Swaps two values using references
template <typename T>
void swapValues(T& a, T& b);

// Returns the absolute value
template <typename T>
T absoluteValue(T value); Demonstrate each template with at least three different data types: int, double, and float.
Part B: Array Processing Templates (10 points)
Implement function templates for array operations:
// Finds and returns the sum of all elements
template <typename T>
T arraySum(const T arr[], int size);

// Finds and returns the average (always returns double)
template <typename T>
double arrayAverage(const T arr[], int size);

// Reverses the array in place
template <typename T>
void reverseArray(T arr[], int size);

// Linear search - returns index or -1 if not found
template <typename T>
int linearSearch(const T arr[], int size, T target);

// Bubble sort in ascending order
template <typename T>
void bubbleSort(T arr[], int size);
You can use code from online for Bubble sort i.e.: https://www.geeksforgeeks.org/cpp/bubble-sort-in-cpp/
Requirements for Part B: - Use for loops for array traversal - Use nested loops for bubble
sort - Include conditional logic for search and sort comparisons
*/

#include <iomanip>
#include <iostream>
#include <string>

/**
 * Displays the elements of an array with a descriptive label.
 *
 * @tparam T Type of elements in the array
 * @param array Array to display
 * @param size Number of elements in the array
 * @param label Descriptive label to print before the array elements
 */
template <typename T>
void printArray(const T array[], int size, const std::string &label)
{
    std::cout << label << ": ";
    for (int i = 0; i < size; i++)
    {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}

/**
 * Returns the larger of two values.
 *
 * @tparam T Type of values to compare (must support > operator)
 * @param a First value to compare
 * @param b Second value to compare
 * @return The larger of the two values
 */
template <typename T>
T maximum(T a, T b)
{
    return (a > b) ? a : b;
}

/**
 * Returns the smaller of two values.
 *
 * @tparam T Type of values to compare (must support < operator)
 * @param a First value to compare
 * @param b Second value to compare
 * @return The smaller of the two values
 */
template <typename T>
T minimum(T a, T b)
{
    return (a < b) ? a : b;
}

/**
 * Swaps the values of two variables.
 *
 * @tparam T Type of values to swap
 * @param a First variable (will be modified)
 * @param b Second variable (will be modified)
 */
template <typename T>
void swapValues(T &a, T &b)
{
    T temp = a;
    a = b;
    b = temp;
}

/**
 * Returns the absolute value of a number.
 *
 * @tparam T Type of value (must support < operator and unary -)
 * @param value Input value
 * @return Absolute value (always non-negative)
 */
template <typename T>
T absoluteValue(T value)
{
    return (value < 0) ? -value : value;
}

/**
 * Calculates the sum of elements in an array.
 *
 * @tparam T Type of elements in the array
 * @param arr Array of elements
 * @param size Number of elements in the array
 * @return Sum of the elements
 */
template <typename T>
T arraySum(const T arr[], int size)
{
    T sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += arr[i];
    }
    return sum;
}

/**
 * Calculates the average of elements in an array.
 *
 * @tparam T Type of elements in the array (must support addition and division)
 * @param arr Array of elements
 * @param size Number of elements in the array
 * @return Average value of the elements
 */
template <typename T>
double arrayAverage(const T arr[], int size)
{
    T sum = arraySum(arr, size);
    return sum / size;
}

/**
 * Reverses the elements of an array in place.
 *
 * @tparam T Type of elements in the array
 * @param arr Array to reverse (will be modified)
 * @param size Number of elements in the array
 */
template <typename T>
void reverseArray(T arr[], int size)
{
    for (int i = 0; i < size / 2; i++)
    {
        swapValues(arr[i], arr[size - 1 - i]);
    }
}

/**
 * Performs a linear search to find a target value in an array.
 *
 * @tparam T Type of elements in the array (must support == operator)
 * @param arr Array to search
 * @param size Number of elements in the array
 * @param target Value to search for
 * @return Index of the target if found, -1 otherwise
 */
template <typename T>
int linearSearch(const T arr[], int size, T target)
{
    for (int i = 0; i < size; i++)
    {
        if (arr[i] == target)
        {
            // Found at index i
            return i;
        }
    }
    // Not found
    return -1;
}

/**
 * Sorts an array in ascending order using the bubble sort algorithm.
 *
 * @tparam T Type of elements in the array (must support > operator)
 * @param arr Array to sort (will be modified in place)
 * @param size Number of elements in the array
 */
template <typename T>
void bubbleSort(T arr[], int size)
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                swapValues(arr[j], arr[j + 1]);
            }
        }
    }
}

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "Part A: Basic Function Templates" << std::endl;
    std::cout << "========================================" << std::endl;

    // Test with int
    std::cout << "\n--- Testing with int ---" << std::endl;
    int a = 10, b = 25;
    std::cout << "a = " << a << ", b = " << b << std::endl;
    std::cout << "maximum(a, b) = " << maximum(a, b) << std::endl;
    std::cout << "minimum(a, b) = " << minimum(a, b) << std::endl;
    std::cout << "absoluteValue(-15) = " << absoluteValue(-15) << std::endl;
    swapValues(a, b);
    std::cout << "After swapValues: a = " << a << ", b = " << b << std::endl;

    // Test with double
    std::cout << "\n--- Testing with double ---" << std::endl;
    double x = 3.14, y = 2.71;
    std::cout << "x = " << x << ", y = " << y << std::endl;
    std::cout << "maximum(x, y) = " << maximum(x, y) << std::endl;
    std::cout << "minimum(x, y) = " << minimum(x, y) << std::endl;
    std::cout << "absoluteValue(-9.8) = " << absoluteValue(-9.8) << std::endl;
    swapValues(x, y);
    std::cout << "After swapValues: x = " << x << ", y = " << y << std::endl;

    // Test with char
    std::cout << "\n--- Testing with char ---" << std::endl;
    char c1 = 'A', c2 = 'Z';
    std::cout << "c1 = '" << c1 << "', c2 = '" << c2 << "'" << std::endl;
    std::cout << "maximum(c1, c2) = '" << maximum(c1, c2) << "'" << std::endl;
    std::cout << "minimum(c1, c2) = '" << minimum(c1, c2) << "'" << std::endl;
    swapValues(c1, c2);
    std::cout << "After swapValues: c1 = '" << c1 << "', c2 = '" << c2 << "'" << std::endl;

    std::cout << "\n========================================" << std::endl;
    std::cout << "Part B: Array Processing Templates" << std::endl;
    std::cout << "========================================" << std::endl;

    // Test with int array
    std::cout << "\n--- Testing with int array ---" << std::endl;
    int intArr[] = {64, 34, 25, 12, 22, 11, 90};
    int intSize = sizeof(intArr) / sizeof(intArr[0]);

    printArray(intArr, intSize, "Original array");
    std::cout << "Sum: " << arraySum(intArr, intSize) << std::endl;
    std::cout << "Average: " << std::fixed << std::setprecision(2) << arrayAverage(intArr, intSize) << std::endl;
    std::cout << "Search for 25: index " << linearSearch(intArr, intSize, 25) << std::endl;
    std::cout << "Search for 99: index " << linearSearch(intArr, intSize, 99) << std::endl;

    bubbleSort(intArr, intSize);
    printArray(intArr, intSize, "After bubbleSort");

    reverseArray(intArr, intSize);
    printArray(intArr, intSize, "After reverseArray");

    // Test with double array
    std::cout << "\n--- Testing with double array ---" << std::endl;
    double dblArr[] = {3.14, 1.41, 2.71, 1.73, 1.62};
    int dblSize = sizeof(dblArr) / sizeof(dblArr[0]);

    printArray(dblArr, dblSize, "Original array");
    std::cout << "Sum: " << arraySum(dblArr, dblSize) << std::endl;
    std::cout << "Average: " << arrayAverage(dblArr, dblSize) << std::endl;

    bubbleSort(dblArr, dblSize);
    printArray(dblArr, dblSize, "After bubbleSort");
}

/*
Sample Output:

========================================
Part A: Basic Function Templates
========================================

--- Testing with int ---
a = 10, b = 25
maximum(a, b) = 25
minimum(a, b) = 10
absoluteValue(-15) = 15
After swapValues: a = 25, b = 10

--- Testing with double ---
x = 3.14, y = 2.71
maximum(x, y) = 3.14
minimum(x, y) = 2.71
absoluteValue(-9.8) = 9.8
After swapValues: x = 2.71, y = 3.14

--- Testing with char ---
c1 = 'A', c2 = 'Z'
maximum(c1, c2) = 'Z'
minimum(c1, c2) = 'A'
After swapValues: c1 = 'Z', c2 = 'A'

========================================
Part B: Array Processing Templates
========================================

--- Testing with int array ---
Original array: 64 34 25 12 22 11 90
Sum: 258
Average: 36.00
Search for 25: index 2
Search for 99: index -1
After bubbleSort: 11 12 22 25 34 64 90
After reverseArray: 90 64 34 25 22 12 11

--- Testing with double array ---
Original array: 3.14 1.41 2.71 1.73 1.62
Sum: 10.61
Average: 2.12
After bubbleSort: 1.41 1.62 1.73 2.71 3.14

*/
