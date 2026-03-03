/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-02-13
 * Description: Homework 2: Ray Tracing Visualization with Multiple Rendering Modes
 *
 * @file Report.cpp
 * @brief Report implementation for CSV performance data logging
 */

#include "Report.h"
#include <array>
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

/** @brief Generate a timestamp string in format _yyyymmdd_hhmmss for filename
 *  @return Timestamp string
 */
static auto generateFileTimestamp() -> std::string
{
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    const std::tm *localTime = const_cast<const std::tm *>(std::localtime(&timeT));

    std::array<char, 20> buffer{};

    // Use timestamp in the name to ensure uniqueness and prevent overwriting previous files
    std::strftime(buffer.data(), buffer.size(), "_%Y%m%d_%H%M%S", localTime);
    return std::string(buffer.data());
}

/** @brief Generate a Unix epoch timestamp in milliseconds for CSV rows
 *  @return Unix timestamp as string (milliseconds since epoch)
 */
static auto generateRowTimestamp() -> std::string
{
    auto now = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return std::to_string(milliseconds.count());
}

/** @brief Get the build mode (Debug or Release)
 *  @return "Debug" or "Release"
 */
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Report::getBuildMode() -> std::string
{
#ifdef BUILD_TYPE_DEBUG
    return "Debug";
#else
    return "Release";
#endif
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
Report::Report(bool enable, int sampleCount) : isOpen(false), sampleCount(sampleCount)
{
    // If CSV reporting is not enabled, do not attempt to open a file
    if (!enable)
    {
        return;
    }

    filename = "performance_" + generateFileTimestamp() + ".csv";
    csvFile.open(filename);

    if (csvFile.is_open())
    {
        // Write CSV header with all columns
        csvFile << "timestamp,renderMode,threadCount,rayCount,elapsedMicroseconds,buildMode\n";
        csvFile.flush();
        isOpen = true;
    }
    else
    {
        throw std::runtime_error("Failed to open " + filename + " for writing");
    }
}

Report::~Report()
{
    if (csvFile.is_open())
    {
        csvFile.close();
        if (isOpen)
        {
            std::cout << "Performance data written to: " << filename << "\n";
        }
    }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Report::writeData(const std::string &renderMode, int threadCount, int rayCount, int32_t elapsedMicroseconds) -> void
{
    if (!isOpenForWriting())
    {
        return;
    }

    std::string key = renderMode + "_" + std::to_string(threadCount) + "_" + std::to_string(rayCount);
    reportingCounts[key]++;

    if (reportingCounts[key] > sampleCount)
    {
        std::cout << "Sample limit reached for " << key << ", skipping further entries.\n";
        return;
    }

    std::string timestamp = generateRowTimestamp();
    csvFile << timestamp << "," << renderMode << "," << threadCount << "," << rayCount << "," << elapsedMicroseconds << ","
            << getBuildMode() << "\n";
    csvFile.flush();
}

auto Report::isOpenForWriting() const -> bool
{
    return isOpen && csvFile.is_open();
}
