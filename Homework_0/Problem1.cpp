/*
Author: Jennifer Cwagenberg
Class: ECE6122
Last Date Modified: 2026-01-22
Description:  Homework 1: Statistical Analysis with Pass-by-Reference

Write a C++ program that analyzes a collection of floating-point sensor readings. Your
program must: 1. Prompt the user to enter the number of readings (between 5 and 100) 2. Dynamically read that many
double values from standard input 3. Implement the following functions that compute statistics using pass-by-reference
for output parameters:
void computeStatistics(const double data[], int size,
                       double& mean, double& variance, double& stdDev);

void findExtrema(const double data[], int size,
                 double& minVal, double& maxVal, int& minIndex,                   int& maxIndex); 4. Display all
computed statistics with appropriate formatting (2 decimal places) Requirements: - Use a for loop to read input values -
Use appropriate loops for statistical calculations - Validate that the number of readings is within the specified range
using conditional statements - The const keyword must be used appropriately to prevent modification of input arrays

*/
#include <cmath>
#include <iomanip>
#include <iostream>

/**
 * Computes statistical measures for sensor data.
 *
 * Calculates the arithmetic mean, variance, and standard deviation
 * for an array of sensor readings.
 *
 * @param data Input array of sensor readings (must not be modified)
 * @param size Number of readings in the array
 * @param mean Output: calculated arithmetic mean
 * @param variance Output: calculated variance
 * @param stdDev Output: calculated standard deviation
 */
void computeStatistics(const double data[], int size, double &mean, double &variance, double &stdDev) {
    double sum = 0.0;
    for (int i = 0; i < size; ++i)
    {
        sum += data[i];
    }
    mean = sum / size;

    double varSum = 0.0;
    for (int i = 0; i < size; ++i)
    {
        varSum += std::pow((data[i] - mean), 2);
    }
    variance = varSum / (size - 1);  // Sample variance (Bessel's correction)
    stdDev = std::sqrt(variance);
}

/**
 * Finds the minimum and maximum values in sensor data.
 *
 * Searches through the data array to identify the smallest and largest
 * values along with their positions.
 *
 * @param data Input array of sensor readings (must not be modified)
 * @param size Number of readings in the array
 * @param minVal Output: minimum value found in the array
 * @param maxVal Output: maximum value found in the array
 * @param minIndex Output: array index of the minimum value
 * @param maxIndex Output: array index of the maximum value
 */
void findExtrema(const double data[], int size, double &minVal, double &maxVal, int &minIndex, int &maxIndex) {
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

int main() {
    std::cout << "Enter number of readings (5-100): " << std::endl;

    // Capture and Validate input for number of readings
    short numReadings;
    std::cin >> numReadings;
    if (std::cin.fail() || numReadings < 5 || numReadings > 100)
    {
        std::cerr << "Error: Number of readings must be between 5 and 100." << std::endl;
        return 1;
    }

    double *readings = new double[numReadings];
    for (int i = 0; i < numReadings; ++i)
    {
        std::cout << "Enter reading " << (i + 1) << ": ";
        std::cin >> readings[i];
        if (std::cin.fail())
        {
            std::cerr << "Error: Invalid input. Please enter a valid floating-point number." << std::endl;
            delete[] readings;
            return 1;
        }
    }

    double mean, variance, stdDev;
    computeStatistics(readings, numReadings, mean, variance, stdDev);

    double minVal, maxVal;
    int minIndex, maxIndex;
    findExtrema(readings, numReadings, minVal, maxVal, minIndex, maxIndex);

    std::cout << "\nStatistical Analysis Results:\n";
    std::cout << "---------------------------\n";
    std::cout << "Mean:              " << std::fixed << std::setprecision(2) << mean << "\n";
    std::cout << "Variance:          " << std::fixed << std::setprecision(2) << variance << "\n";
    std::cout << "Standard Deviation: " << std::fixed << std::setprecision(2) << stdDev << "\n";
    std::cout << "Minimum Value:     " << std::fixed << std::setprecision(2) << minVal << " (at index " << minIndex
              << ")\n";
    std::cout << "Maximum Value:     " << std::fixed << std::setprecision(2) << maxVal << " (at index " << maxIndex
              << ")\n";

    delete[] readings;
    return 0;
}