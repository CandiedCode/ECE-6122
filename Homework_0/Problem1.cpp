/*
Author: Jennifer Cwagenberg
Class: ECE6122
Last Date Modified: 2026-01-22
Description:  Homework 1: Statistical Analysis with Pass-by-Reference

Write a C++ program that analyzes a collection of floating-point sensor readings. Your
program must:
1. Prompt the user to enter the number of readings (between 5 and 100)
2. Dynamically read that many double values from standard input
3. Implement the following functions that compute statistics using pass-by-reference for output parameters:
  void computeStatistics(const double data[], int size, double& mean, double& variance, double& stdDev);
  void findExtrema(const double data[], int size, double& minVal, double& maxVal, int& minIndex, int& maxIndex);
4. Display all computed statistics with appropriate formatting (2 decimal places)

Requirements:
- Use a for loop to read input values
- Use appropriate loops for statistical calculations
- Validate that the number of readings is within the specified range using conditional statements
- The const keyword must be used appropriately to prevent modification of input arrays

*/
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>

/**
 * Calculates the arithmetic mean, variance, and standard deviation
 * for an array of sensor readings.
 *
 * @param data Input array of sensor readings
 * @param size Number of readings in the array
 * @param mean Output: calculated arithmetic mean
 * @param variance Output: calculated sample variance
 * @param stdDev Output: calculated standard deviation
 */
void computeStatistics(const double data[], int size, double &mean, double &variance, double &stdDev)
{
    // Add size validation
    if (size < 2)
    {
        mean = (size == 1) ? data[0] : 0.0;
        variance = 0.0;
        stdDev = 0.0;
        return;
    }

    double sum = 0.0;
    for (int i = 0; i < size; ++i)
    {
        sum += data[i];
    }
    mean = sum / size;

    double varSum = 0.0;
    for (int i = 0; i < size; ++i)
    {
        varSum += (data[i] - mean) * (data[i] - mean);
    }
    variance = varSum / (size - 1);
    stdDev = std::sqrt(variance);
}

/**
 * Finds the minimum and maximum values in sensor data.
 *
 * @param data Input array of sensor readings
 * @param size Number of readings in the array
 * @param minVal Output: minimum value found in the array
 * @param maxVal Output: maximum value found in the array
 * @param minIndex Output: array index of the minimum value
 * @param maxIndex Output: array index of the maximum value
 */
void findExtrema(const double data[], int size, double &minVal, double &maxVal, int &minIndex, int &maxIndex)
{
    minVal = data[0];
    maxVal = data[0];
    minIndex = 0;
    maxIndex = 0;

    for (int i = 1; i < size; i++)
    {
        if (data[i] < minVal)
        {
            minVal = data[i];
            minIndex = i;
        }
        if (data[i] > maxVal)
        {
            maxVal = data[i];
            maxIndex = i;
        }
    }
}

int main()
{
    const int MIN_READINGS = 5;
    const int MAX_READINGS = 100;
    std::cout << "Enter number of readings (" << MIN_READINGS << "-" << MAX_READINGS << "): " << std::endl;

    // Capture and Validate input for number of readings
    int numReadings;
    std::cin >> numReadings;
    while (std::cin.fail() || numReadings < 5 || numReadings > 100)
    {
        std::cerr << "Error: Input is invalid." << std::endl;
        std::cout << "Enter number of readings (" << MIN_READINGS << "-" << MAX_READINGS << "): " << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin >> numReadings;
    }

    double *readings = new double[numReadings];
    for (int i = 0; i < numReadings; ++i)
    {
        std::cout << "Enter reading " << (i + 1) << ": ";
        std::cin >> readings[i];
        while (std::cin.fail())
        {
            std::cerr << "Error: Invalid input. Please enter a valid floating-point number." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Enter reading " << (i + 1) << ": ";
            std::cin >> readings[i];
        }
    }

    double mean, variance, stdDev;
    computeStatistics(readings, numReadings, mean, variance, stdDev);

    double minVal, maxVal;
    int minIndex, maxIndex;
    findExtrema(readings, numReadings, minVal, maxVal, minIndex, maxIndex);

    std::cout << "\nStatistical Analysis Results:\n";
    std::cout << "---------------------------\n";
    std::cout << "Mean:              " << std::fixed << std::setprecision(2) << mean << std::endl;
    std::cout << "Variance:          " << std::fixed << std::setprecision(2) << variance << std::endl;
    std::cout << "Standard Deviation: " << std::fixed << std::setprecision(2) << stdDev << std::endl;
    std::cout << "Minimum Value:     " << std::fixed << std::setprecision(2) << minVal << " (at index " << minIndex
              << ")" << std::endl;
    std::cout << "Maximum Value:     " << std::fixed << std::setprecision(2) << maxVal << " (at index " << maxIndex
              << ")" << std::endl;

    delete[] readings;
    return 0;
}

/*
Sample Output:

Enter number of readings (5-100):
blag
Error: Input is invalid.
Enter number of readings (5-100):
4
Error: Input is invalid.
Enter number of readings (5-100):
6
Enter reading 1: 23.5
Enter reading 2: 18.2
Enter reading 3: 31.7
Enter reading 4: 25.0
Enter reading 5: 19.8
Enter reading 6: 27.3

Statistical Analysis Results:
---------------------------
Mean:              24.25
Variance:          24.47
Standard Deviation: 4.95
Minimum Value:     18.20 (at index 1)
Maximum Value:     31.70 (at index 2)

 */
