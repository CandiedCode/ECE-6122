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
Requirements: - Include comments explaining why the array version of pass-by-value
doesn’t work as expected in C++.

*/

#include <iostream>

// Struct to represent a 3x3 matrix
struct Matrix3x3 {
    double data[3][3];
};

/// <summary>
/// Prints a 3x3 matrix to standard output.
/// Displays the matrix in a formatted grid with tab-separated values.
/// </summary>
/// <param name="matrix">Input 3x3 array to display</param>
void printMatrix(const double matrix[3][3]) {
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

/// <summary>
/// Resets a matrix to its original values by copying from a source matrix.
/// Copies all elements from the original matrix to the target matrix,
/// restoring it to a previous state.
/// </summary>
/// <param name="matrix">3x3 array to reset (destination, will be modified)</param>
/// <param name="original">3x3 array containing original values (source, read-only)</param>
void resetMatrix(double matrix[3][3], const double original[3][3]) {
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            matrix[i][j] = original[i][j];
        }
    }
}

/// <summary>
/// Compares two 3x3 matrices for equality.
/// Checks if all corresponding elements in the two matrices are equal.
/// </summary>
/// <param name="mat1">First 3x3 array to compare</param>
/// <param name="mat2">Second 3x3 array to compare</param>
/// <returns>true if matrices are equal, false otherwise</returns>
bool matrixEqual(const double mat1[3][3], const double mat2[3][3]) {
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (mat1[i][j] != mat2[i][j])
            {
                return false;
            }
        }
    }
    return true;
}

/// <summary>
/// Attempts to scale a matrix by a scalar value (misleading "by value").
/// Despite the name suggesting pass-by-value, arrays in C++ decay to pointers,
/// so this function WILL modify the original matrix. This demonstrates that
/// true pass-by-value is not possible for arrays in C++ without wrapping them
/// in a struct or using explicit copying.
/// </summary>
/// <param name="matrix">3x3 array to scale (decays to pointer, so original IS modified)</param>
/// <param name="scalar">Value to multiply each element by</param>
void scaleMatrixByValue(double matrix[3][3], double scalar) {
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            matrix[i][j] *= scalar;
        }
    }
}

/// <summary>
/// Scales a matrix by a scalar value using pass-by-reference.
/// This function uses explicit pass-by-reference syntax with the & operator,
/// making it clear that the original matrix WILL be modified. This is the
/// proper way to explicitly document intent to modify the caller's data.
/// </summary>
/// <param name="matrix">3x3 array to scale (passed by reference, original IS modified)</param>
/// <param name="scalar">Value to multiply each element by</param>
void scaleMatrixByReference(double (&matrix)[3][3], double scalar) {
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            matrix[i][j] *= scalar;
        }
    }
}

/// <summary>
/// Scales a matrix struct by a scalar value using true pass-by-value.
/// Unlike arrays, structs CAN be passed by value in C++. This creates
/// a complete copy of the struct, so modifications to the parameter 'm'
/// do NOT affect the original. The scaled copy is returned, leaving the
/// original unchanged. This is the proper way to achieve pass-by-value
/// semantics with matrix data.
/// </summary>
/// <param name="m">Matrix3x3 struct to scale (passed by value, copy is made)</param>
/// <param name="scalar">Value to multiply each element by</param>
/// <returns>New scaled matrix (original is unchanged)</returns>
Matrix3x3 scaleMatrixCopy(Matrix3x3 m, double scalar) {
    scaleMatrixByValue(m.data, scalar);
    return m;
}

/// <summary>
/// Scales a matrix struct in place using pass-by-reference.
/// Takes a Matrix3x3 struct by reference (using &), which allows direct
/// modification of the original struct. This demonstrates that structs,
/// like other types, can be passed by reference to avoid copying and to
/// allow in-place modification of the caller's data.
/// </summary>
/// <param name="m">Matrix3x3 struct to scale (passed by reference, original IS modified)</param>
/// <param name="scalar">Value to multiply each element by</param>
void scaleMatrixInPlace(Matrix3x3 &m, double scalar) {
    scaleMatrixByReference(m.data, scalar);
}

int main() {
    double matrix[3][3];
    Matrix3x3 matrixStruct;

    std::cout << "Populate a 3x3 matrix: " << std::endl;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            std::cout << "Enter reading row " << (i + 1) << ", column " << (j + 1) << ": ";
            std::cin >> matrix[i][j];
            if (std::cin.fail())
            {
                std::cerr << "Error: Invalid input. Please enter a valid floating-point number." << std::endl;
                return 1;
            }
            matrixStruct.data[i][j] = matrix[i][j];
        }
    }

    std::cout << "\nOriginal Matrix:\n";
    printMatrix(matrix);

    double scalar;
    std::cout << "\nEnter scaling factor: ";
    std::cin >> scalar;
    if (std::cin.fail())
    {
        std::cerr << "Error: Invalid input. Please enter a valid floating-point number." << std::endl;
        return 1;
    }

    std::cout << "\nScale by " << scalar << "\n";
    // Pass-by-value: Returns a scaled copy, original unchanged
    scaleMatrixByValue(matrix, scalar);
    std::cout << "\nMatrix after scaleMatrixByValue:\n";
    printMatrix(matrix);
    std::cout << "Matrix changed: " << (matrixEqual(matrix, matrixStruct.data) ? "No" : "Yes") << std::endl;
    // Reset to original for next test
    resetMatrix(matrix, matrixStruct.data);

    // Pass-by-reference: Modifies the original matrix in place
    scaleMatrixByReference(matrix, scalar);
    std::cout << "\nMatrix after scaleMatrixByReference:\n";
    printMatrix(matrix);
    std::cout << "Matrix changed: " << (matrixEqual(matrix, matrixStruct.data) ? "No" : "Yes") << std::endl;
    // Reset to original for next test
    resetMatrix(matrix, matrixStruct.data);

    // By value
    Matrix3x3 scaledMatrixStruct = scaleMatrixCopy(matrixStruct, scalar);
    std::cout << "\nScaled Matrix Struct (by value):\n";
    printMatrix(scaledMatrixStruct.data);
    std::cout << "Original Struct Matrix changed: " << (matrixEqual(matrixStruct.data, matrix) ? "No" : "Yes")
              << std::endl;
    std::cout << "Scaled Struct Matrix changed: "
              << (matrixEqual(matrixStruct.data, scaledMatrixStruct.data) ? "No" : "Yes") << std::endl;

    // By reference
    scaleMatrixInPlace(matrixStruct, scalar);
    std::cout << "\nMatrix Struct after scaleMatrixInPlace (should be changed):\n";
    printMatrix(matrixStruct.data);
    std::cout << "Original Struct Matrix changed: " << (matrixEqual(matrixStruct.data, matrix) ? "No" : "Yes")
              << std::endl;

    return 0;
}
