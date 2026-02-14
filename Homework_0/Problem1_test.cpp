/*
Author: Jennifer Cwagenberg
Class: ECE6122
Last Date Modified: 2026-01-22
Description: Unit tests for Problem1.cpp statistical functions
*/

#include <cmath>
#include <gtest/gtest.h>

// Forward declarations of functions to test
void computeStatistics(const double data[], int size, double &mean, double &variance, double &stdDev);
void findExtrema(const double data[], int size, double &minVal, double &maxVal, int &minIndex, int &maxIndex);

// Helper function to compare floating-point values
const double EPSILON = 0.01;

// Tests for computeStatistics
TEST(ComputeStatisticsTest, SampleData)
{
    double data[] = {23.5, 18.2, 31.7, 25.0, 19.8, 27.3};
    int size = 6;
    double mean, variance, stdDev;

    computeStatistics(data, size, mean, variance, stdDev);

    EXPECT_NEAR(mean, 24.25, EPSILON);
    EXPECT_NEAR(variance, 22.69, EPSILON);
    EXPECT_NEAR(stdDev, 4.76, EPSILON);
}

TEST(ComputeStatisticsTest, AllSameValues)
{
    double data[] = {10.0, 10.0, 10.0, 10.0, 10.0};
    int size = 5;
    double mean, variance, stdDev;

    computeStatistics(data, size, mean, variance, stdDev);

    EXPECT_NEAR(mean, 10.0, EPSILON);
    EXPECT_NEAR(variance, 0.0, EPSILON);
    EXPECT_NEAR(stdDev, 0.0, EPSILON);
}

TEST(ComputeStatisticsTest, SimpleSequence)
{
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    int size = 5;
    double mean, variance, stdDev;

    computeStatistics(data, size, mean, variance, stdDev);

    EXPECT_NEAR(mean, 3.0, EPSILON);
    EXPECT_NEAR(variance, 2.0, EPSILON);
    EXPECT_NEAR(stdDev, 1.414, EPSILON);
}

TEST(ComputeStatisticsTest, NegativeValues)
{
    double data[] = {-5.0, -3.0, -1.0, 1.0, 3.0, 5.0};
    int size = 6;
    double mean, variance, stdDev;

    computeStatistics(data, size, mean, variance, stdDev);

    EXPECT_NEAR(mean, 0.0, EPSILON);
    EXPECT_NEAR(variance, 11.67, EPSILON);
    EXPECT_NEAR(stdDev, 3.42, EPSILON);
}

// Tests for findExtrema
TEST(FindExtremaTest, SampleData)
{
    double data[] = {23.5, 18.2, 31.7, 25.0, 19.8, 27.3};
    int size = 6;
    double minVal, maxVal;
    int minIndex, maxIndex;

    findExtrema(data, size, minVal, maxVal, minIndex, maxIndex);

    EXPECT_NEAR(minVal, 18.2, EPSILON);
    EXPECT_NEAR(maxVal, 31.7, EPSILON);
    EXPECT_EQ(minIndex, 1);
    EXPECT_EQ(maxIndex, 2);
}

TEST(FindExtremaTest, MinAtStart)
{
    double data[] = {1.0, 5.0, 3.0, 4.0, 2.0};
    int size = 5;
    double minVal, maxVal;
    int minIndex, maxIndex;

    findExtrema(data, size, minVal, maxVal, minIndex, maxIndex);

    EXPECT_NEAR(minVal, 1.0, EPSILON);
    EXPECT_NEAR(maxVal, 5.0, EPSILON);
    EXPECT_EQ(minIndex, 0);
    EXPECT_EQ(maxIndex, 1);
}

TEST(FindExtremaTest, MaxAtEnd)
{
    double data[] = {3.0, 2.0, 1.0, 4.0, 10.0};
    int size = 5;
    double minVal, maxVal;
    int minIndex, maxIndex;

    findExtrema(data, size, minVal, maxVal, minIndex, maxIndex);

    EXPECT_NEAR(minVal, 1.0, EPSILON);
    EXPECT_NEAR(maxVal, 10.0, EPSILON);
    EXPECT_EQ(minIndex, 2);
    EXPECT_EQ(maxIndex, 4);
}

TEST(FindExtremaTest, AllSameValues)
{
    double data[] = {5.5, 5.5, 5.5, 5.5};
    int size = 4;
    double minVal, maxVal;
    int minIndex, maxIndex;

    findExtrema(data, size, minVal, maxVal, minIndex, maxIndex);

    EXPECT_NEAR(minVal, 5.5, EPSILON);
    EXPECT_NEAR(maxVal, 5.5, EPSILON);
    EXPECT_EQ(minIndex, 0);
    EXPECT_EQ(maxIndex, 0);
}

TEST(FindExtremaTest, NegativeValues)
{
    double data[] = {-10.0, -5.0, -15.0, -2.0, -8.0};
    int size = 5;
    double minVal, maxVal;
    int minIndex, maxIndex;

    findExtrema(data, size, minVal, maxVal, minIndex, maxIndex);

    EXPECT_NEAR(minVal, -15.0, EPSILON);
    EXPECT_NEAR(maxVal, -2.0, EPSILON);
    EXPECT_EQ(minIndex, 2);
    EXPECT_EQ(maxIndex, 3);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
