/*
Author: Jennifer Cwagenberg
Class: ECE6122
Last Date Modified: 2026-01-22
Description:  Problem 2: Matrix Operations with Pass-by-Value vs Pass-by-Reference

Write a program that demonstrates the difference between pass-by-value and pass-by-
reference through matrix operations. Implement the following: 1. A 3x3 matrix represented as a 2D array 2. The following
functions:
// Pass-by-value: Returns a scaled copy, original unchanged
void scaleMatrixByValue(double matrix[3][3], double scalar);

// Pass-by-reference: Modifies the original matrix in place
void scaleMatrixByReference(double (&matrix)[3][3], double scalar);

// Demonstrate with a struct
struct Matrix3x3 {
    double data[3][3];
};

Matrix3x3 scaleMatrixCopy(Matrix3x3 m, double scalar);      // By value
void scaleMatrixInPlace(Matrix3x3& m, double scalar);        // By reference 3. Your main() function must: – Query the
user to initialize the 3x3 matrix with values – Call each function and display the matrix before and after each call –
Use nested for loops for matrix traversal – Clearly demonstrate (with printed output) which functions modify the
original and which do not
Requirements: - Include comments explaining why the array version of pass-by-value doesn’t work as expected in C++.

*/

#include <cmath>
#include <iostream>
#include <limits>

// Struct to represent a 3x3 matrix
struct Matrix3x3
{
    double data[3][3];
};

/**
 * Displays the matrix in a formatted grid with tab-separated values.
 *
 * @param matrix Input 3x3 array to display
 */
void printMatrix(const double matrix[3][3])
{
    // Iterate through each row
    for (int i = 0; i < 3; i++)
    {
        // Iterate through each column in the current row
        for (int j = 0; j < 3; j++)
        {
            std::cout << "  " << matrix[i][j] << "\t";
        }
        // Move to next line after each row
        std::cout << std::endl;
    }
}

/**
 * Copies all elements from the original matrix to the target matrix,
 * restoring it to a previous state.
 *
 * @param matrix 3x3 array to reset (destination, will be modified)
 * @param original 3x3 array containing original values (source, read-only)
 */
void resetMatrix(double matrix[3][3], const double original[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            matrix[i][j] = original[i][j];
        }
    }
}

/**
 * Compares two 3x3 matrices for equality.
 *
 * @param mat1 First 3x3 array to compare
 * @param mat2 Second 3x3 array to compare
 * @return true if matrices are equal, false otherwise
 */
bool matrixEqual(const double mat1[3][3], const double mat2[3][3])
{
    const double EPSILON = 1e-9;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (std::abs(mat1[i][j] - mat2[i][j]) > EPSILON)
            {
                return false;
            }
        }
    }
    return true;
}

/**
 * Attempts to scale a matrix by a scalar value (misleading "by value").
 *
 * Despite the name suggesting pass-by-value, arrays in C++ decay to pointers,
 * so this function WILL modify the original matrix. This demonstrates that
 * true pass-by-value is not possible for arrays in C++ without wrapping them
 * in a struct or using explicit copying.
 *
 * @param matrix 3x3 array to scale (decays to pointer, so original IS modified)
 * @param scalar Value to multiply each element by
 */
void scaleMatrixByValue(double matrix[3][3], double scalar)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            matrix[i][j] *= scalar;
        }
    }
}

/**
 * Scales a matrix by a scalar value using pass-by-reference.
 *
 * This function uses explicit pass-by-reference syntax with the & operator,
 * making it clear that the original matrix WILL be modified.
 *
 * @param matrix 3x3 array to scale
 * @param scalar Value to multiply each element by
 */
void scaleMatrixByReference(double (&matrix)[3][3], double scalar)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            matrix[i][j] *= scalar;
        }
    }
}

/**
 * Scales a matrix struct by a scalar value using true pass-by-value.
 *
 * Unlike arrays, structs CAN be passed by value in C++. This creates
 * a complete copy of the struct, so modifications to the parameter 'm'
 * do NOT affect the original. The scaled copy is returned, leaving the
 * original unchanged. This is the proper way to achieve pass-by-value
 * semantics with matrix data.
 *
 * @param m Matrix3x3 struct to scale (passed by value, copy is made)
 * @param scalar Value to multiply each element by
 * @return New scaled matrix (original is unchanged)
 */
Matrix3x3 scaleMatrixCopy(Matrix3x3 m, double scalar)
{
    scaleMatrixByValue(m.data, scalar);
    return m;
}

/**
 * Scales a matrix struct in place using pass-by-reference.
 *
 * Takes a Matrix3x3 struct by reference (using &), which allows direct
 * modification of the original struct. This demonstrates that structs,
 * like other types, can be passed by reference to avoid copying and to
 * allow in-place modification of the caller's data.
 *
 * @param m Matrix3x3 struct to scale (passed by reference, original IS modified)
 * @param scalar Value to multiply each element by
 */
void scaleMatrixInPlace(Matrix3x3 &m, double scalar)
{
    scaleMatrixByReference(m.data, scalar);
}

int main()
{
    double matrix[3][3];
    Matrix3x3 matrixStruct;

    std::cout << "Populate a 3x3 matrix: " << std::endl;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            std::cout << "Enter reading row " << (i + 1) << ", column " << (j + 1) << ": ";
            std::cin >> matrix[i][j];
            while (std::cin.fail())
            {
                std::cerr << "Error: Invalid input. Please enter a valid floating-point number." << std::endl;
                std::cout << "Enter reading row " << (i + 1) << ", column " << (j + 1) << ": ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin >> matrix[i][j];
            }
            matrixStruct.data[i][j] = matrix[i][j];
        }
    }

    std::cout << "\nOriginal Matrix:\n";
    printMatrix(matrix);

    double scalar;
    std::cout << "Enter scaling factor: ";
    std::cin >> scalar;
    while (std::cin.fail())
    {
        std::cerr << "Error: Invalid input. Please enter a valid floating-point number." << std::endl;
        std::cout << "Enter scaling factor: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin >> scalar;
    }

    std::cout << std::endl;
    // Pass-by-value: The original matrix will be modified due to array decay.
    std::cout << "== Pass-by-Value Matrix Scaling ==" << std::endl;
    std::cout << "Matrix before scaleMatrixByValue:" << std::endl;
    printMatrix(matrix);
    scaleMatrixByValue(matrix, scalar);
    std::cout << "Matrix after scaleMatrixByValue:" << std::endl;
    printMatrix(matrix);
    std::cout << "Input Matrix modified: " << (!matrixEqual(matrix, matrixStruct.data) ? "Yes" : "No") << std::endl;
    // Reset to original for next test
    resetMatrix(matrix, matrixStruct.data);

    std::cout << std::endl;
    // Pass-by-reference: Modifies the original matrix in place
    std::cout << "== Pass-by-Reference Matrix Scaling ==" << std::endl;
    std::cout << "Matrix before scaleMatrixByReference:" << std::endl;
    printMatrix(matrix);
    scaleMatrixByReference(matrix, scalar);
    std::cout << "Matrix after scaleMatrixByReference:" << std::endl;
    printMatrix(matrix);
    std::cout << "Input Matrix modified: " << (!matrixEqual(matrix, matrixStruct.data) ? "Yes" : "No") << std::endl;
    // Reset to original for next test
    resetMatrix(matrix, matrixStruct.data);

    std::cout << std::endl;
    // Pass-by-value: Modifies a copy, original unchanged
    std::cout << "== Pass-by-Value Struct Matrix Scaling(scaleMatrixCopy): ==" << std::endl;
    std::cout << "Matrix before scaleMatrixCopy:" << std::endl;
    printMatrix(matrixStruct.data);
    Matrix3x3 scaledMatrixStruct = scaleMatrixCopy(matrixStruct, scalar);
    std::cout << "Matrix after scaleMatrixCopy:" << std::endl;
    printMatrix(matrixStruct.data);
    std::cout << "Scaled Matrix Struct Copy (by value):" << std::endl;
    printMatrix(scaledMatrixStruct.data);
    std::cout << "Original Struct Matrix modified: " << (!matrixEqual(matrixStruct.data, matrix) ? "Yes" : "No")
              << std::endl;

    std::cout << std::endl;
    // Pass-by-reference: Modifies the original struct in place
    std::cout << "== Pass-by-reference Struct Matrix Scaling(scaleMatrixInPlace): ==" << std::endl;
    std::cout << "Matrix before scaleMatrixInPlace:" << std::endl;
    printMatrix(matrixStruct.data);
    scaleMatrixInPlace(matrixStruct, scalar);
    std::cout << "Matrix Struct after scaleMatrixInPlace:" << std::endl;
    printMatrix(matrixStruct.data);
    std::cout << "Original Struct Matrix modified: " << (!matrixEqual(matrixStruct.data, matrix) ? "Yes" : "No")
              << std::endl;
    return 0;
}

/*
Sample Output:

Populate a 3x3 matrix:
Enter reading row 1, column 1: g
Error: Invalid input. Please enter a valid floating-point number.
Enter reading row 1, column 1: 1
Enter reading row 1, column 2: 2
Enter reading row 1, column 3: 3
Enter reading row 2, column 1: 1.1
Enter reading row 2, column 2: 2.2
Enter reading row 2, column 3: 3.3
Enter reading row 3, column 1: 1.11
Enter reading row 3, column 2: 2.22
Enter reading row 3, column 3: 3.33

Original Matrix:
  1       2       3
  1.1     2.2     3.3
  1.11    2.22    3.33
Enter scaling factor: 5

== Pass-by-Value Matrix Scaling ==
Matrix before scaleMatrixByValue:
  1       2       3
  1.1     2.2     3.3
  1.11    2.22    3.33
Matrix after scaleMatrixByValue:
  5       10      15
  5.5     11      16.5
  5.55    11.1    16.65
Input Matrix modified: Yes

== Pass-by-Reference Matrix Scaling ==
Matrix before scaleMatrixByReference:
  1       2       3
  1.1     2.2     3.3
  1.11    2.22    3.33
Matrix after scaleMatrixByReference:
  5       10      15
  5.5     11      16.5
  5.55    11.1    16.65
Input Matrix modified: Yes

== Pass-by-Value Struct Matrix Scaling(scaleMatrixCopy): ==
Matrix before scaleMatrixCopy:
  1       2       3
  1.1     2.2     3.3
  1.11    2.22    3.33
Matrix after scaleMatrixCopy:
  1       2       3
  1.1     2.2     3.3
  1.11    2.22    3.33
Scaled Matrix Struct Copy (by value):
  5       10      15
  5.5     11      16.5
  5.55    11.1    16.65
Original Struct Matrix modified: No

== Pass-by-reference Struct Matrix Scaling(scaleMatrixInPlace): ==
Matrix before scaleMatrixInPlace:
  1       2       3
  1.1     2.2     3.3
  1.11    2.22    3.33
Matrix Struct after scaleMatrixInPlace:
  5       10      15
  5.5     11      16.5
  5.55    11.1    16.65
Original Struct Matrix modified: Yes

*/
