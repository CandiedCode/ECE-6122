/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-02-13
 * Description: Homework 2: Ray Tracing Visualization with Multiple Rendering Modes
 *
 * @file Report.h
 * @brief Report header file for CSV performance data logging
 */

#ifndef HOMEWORK_2_REPORT_H_
#define HOMEWORK_2_REPORT_H_

#include <cstdint>
#include <fstream>
#include <string>
#include <unordered_map>

/** @class Report
 *  @brief Manages performance data reporting to CSV files
 */
class Report
{
  private:
    std::ofstream csvFile;
    std::string filename;
    int sampleCount;
    std::unordered_map<std::string, int> reportingCounts;
    bool isOpen{false};

  public:
    /** @brief Construct a Report object
     *  @param enable Whether to enable CSV reporting
     *  @param sampleCount Number of samples per configuration
     */
    explicit Report(bool enable = false, int sampleCount = 100);

    /** @brief Destructor that closes the file if open
     */
    ~Report();

    // Delete copy and move operations as Report manages file resources
    Report(const Report &) = delete;
    auto operator=(const Report &) -> Report & = delete;
    Report(Report &&) = delete;
    auto operator=(Report &&) -> Report & = delete;

    /** @brief Write a performance measurement to the CSV file
     *  @param renderMode The rendering mode as string
     *  @param threadCount Number of threads used
     *  @param rayCount Number of rays cast
     *  @param elapsedMicroseconds Elapsed time in microseconds
     */
    auto writeData(const std::string &renderMode, int threadCount, int rayCount, int32_t elapsedMicroseconds) -> void;

    /** @brief Check if the report file is open
     *  @return True if file is open and ready for writing
     */
    [[nodiscard]] auto isOpenForWriting() const -> bool;

  private:
    /** @brief Get the build mode (Debug or Release)
     *  @return "Debug" or "Release"
     */
    [[nodiscard]] static auto getBuildMode() -> std::string;

    /** @brief Get the optimization level
     *  @return "O0", "O1", "O2", "O3", "Os", or "Unknown"
     */
    [[nodiscard]] static auto getOptimizationLevel() -> std::string;
};

#endif // HOMEWORK_2_REPORT_H_
