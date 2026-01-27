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

#include <iostream>

/**
 * Returns the larger of two values.
 *
 * @tparam T Type of values to compare (must support > operator)
 * @param a First value to compare
 * @param b Second value to compare
 * @return The larger of the two values
 */
template <typename T>
T maximum(T a, T b) {
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
T minimum(T a, T b) {
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
void swapValues(T &a, T &b) {
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
T absoluteValue(T value) {
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
T arraySum(const T arr[], int size) {
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
double arrayAverage(const T arr[], int size) {
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
void reverseArray(T arr[], int size) {
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
int linearSearch(const T arr[], int size, T target) {
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
void bubbleSort(T arr[], int size) {
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

int main() {
    int a = -10, b = 5;
    std::cout << "Using a: " << a << ", b: " << b << std::endl;
    std::cout << "  Max(a,b):\t" << maximum(a, b) << std::endl;
    std::cout << "  Min(a,b):\t" << minimum(a, b) << std::endl;
    std::cout << "  Absolute value (a): " << absoluteValue(a) << std::endl;
    swapValues(a, b);
    std::cout << "  After swap (a,b), a: " << a << ", b: " << b << std::endl;
    std::cout << std::endl;

    double x = -3.14, y = 2.71;
    std::cout << "Using x: " << x << ", y: " << y << std::endl;
    std::cout << "  Max (x,y):\t" << maximum(x, y) << std::endl;
    std::cout << "  Min (x,y):\t" << minimum(x, y) << std::endl;
    std::cout << "  Absolute value (x): " << absoluteValue(x) << std::endl;
    swapValues(x, y);
    std::cout << "  After swap (x,y), x: " << x << ", y: " << y << std::endl;
    std::cout << std::endl;

    float p = -1.5f, q = 4.5f;
    std::cout << "Using p: " << p << ", q: " << q << std::endl;
    std::cout << "  Max (p,q):\t" << maximum(p, q) << std::endl;
    std::cout << "  Min (p,q):\t" << minimum(p, q) << std::endl;
    std::cout << "  Absolute value (p): " << absoluteValue(p) << std::endl;
    swapValues(p, q);
    std::cout << "  After swap (p,q), p: " << p << ", q: " << q << std::endl;
    std::cout << std::endl;

    int intArr[] = {5, 2, 9, 1, 5, 6};
    float floatArray[] = {3.5f, 2.1f, 4.7f, 1.9f};
    double doubleArray[] = {2.2, 3.3, 1.1, 4.4, 5.5};

    std::cout << "Integer array operations:" << std::endl;
    std::cout << "  Sum: " << arraySum(intArr, 6) << std::endl;
    std::cout << "  Average: " << arrayAverage(intArr, 6) << std::endl;
    reverseArray(intArr, 6);
    std::cout << "  Reversed array: ";
    for (int val : intArr)
        std::cout << val << " ";
    std::cout << std::endl;
    int searchTarget = 9;
    int searchIndex = linearSearch(intArr, 6, searchTarget);
    std::cout << "  Linear search for " << searchTarget << ": "
              << ((searchIndex != -1) ? "Found at index " + std::to_string(searchIndex) : "Not found") << std::endl;
    bubbleSort(intArr, 6);
    std::cout << "  Sorted array: ";
    for (int val : intArr)
        std::cout << val << " ";
    std::cout << std::endl << std::endl;

    std::cout << "Float array operations:" << std::endl;
    std::cout << "  Sum: " << arraySum(floatArray, 4) << std::endl;
    std::cout << "  Average: " << arrayAverage(floatArray, 4) << std::endl;
    reverseArray(floatArray, 4);
    std::cout << "  Reversed array: ";
    for (float val : floatArray)
        std::cout << val << " ";
    std::cout << std::endl;
    float floatSearchTarget = 4.7f;
    int floatSearchIndex = linearSearch(floatArray, 4, floatSearchTarget);
    std::cout << "  Linear search for " << floatSearchTarget << ": "
              << ((floatSearchIndex != -1) ? "Found at index " + std::to_string(floatSearchIndex) : "Not found")
              << std::endl;
    bubbleSort(floatArray, 4);
    std::cout << "  Sorted array: ";
    for (float val : floatArray)
        std::cout << val << " ";
    std::cout << std::endl << std::endl;

    std::cout << "Double array operations:" << std::endl;
    std::cout << "  Sum: " << arraySum(doubleArray, 5) << std::endl;
    std::cout << "  Average: " << arrayAverage(doubleArray, 5) << std::endl;
    reverseArray(doubleArray, 5);
    std::cout << "  Reversed array: ";
    for (double val : doubleArray)
        std::cout << val << " ";
    std::cout << std::endl;
    double doubleSearchTarget = 3.3;
    int doubleSearchIndex = linearSearch(doubleArray, 5, doubleSearchTarget);
    std::cout << "  Linear search for " << doubleSearchTarget << ": "
              << ((doubleSearchIndex != -1) ? "Found at index " + std::to_string(doubleSearchIndex) : "Not found")
              << std::endl;
    bubbleSort(doubleArray, 5);
    std::cout << "  Sorted array: ";
    for (double val : doubleArray)
        std::cout << val << " ";
    std::cout << std::endl;

    return 0;
}
