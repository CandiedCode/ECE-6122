/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-02-13
 * Description:  Homework 2: Ray Tracing Visualization with Multiple Rendering Modes
 *
 *
 * @file RayTracer.cpp
 * @brief Ray tracing calculations for visualization. Determines ray intersections with various shapes.  These functions were provided as
 * part of Homework 2 artifacts.
 */

#include "RayTracer.h"
#include "Scene.h"
#include <algorithm>
#include <cmath>
#include <omp.h>
#include <thread>
#include <vector>

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto RayTracer::castRaysSingleThreaded(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results)
    -> void
{
    results.resize(numRays);
    Ray ray;
    ray.origin = lightPos;
    for (int i = 0; i < numRays; ++i)
    {
        auto angle = (2.0F * static_cast<float>(M_PI) * i) / numRays;
        ray.direction = {std::cos(angle), std::sin(angle)};
        results[i] = scene.closestIntersection(ray);
    }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto RayTracer::castRaysOpenMP(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results,
                               int numThreads) -> void
{
    results.resize(numRays);
    omp_set_num_threads(numThreads);
#pragma omp parallel for schedule(static)
    for (int i = 0; i < numRays; ++i)
    {
        auto angle = (2.0F * static_cast<float>(M_PI) * i) / numRays;
        Ray ray;
        ray.origin = lightPos;
        ray.direction = {std::cos(angle), std::sin(angle)};
        results[i] = scene.closestIntersection(ray);
    }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto RayTracer::castRaysStdThread(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results,
                                  int numThreads) -> void
{
    results.resize(numRays);
    std::vector<std::thread> threads;
    int chunkSize = numRays / numThreads;
    for (int threadIdx = 0; threadIdx < numThreads; ++threadIdx)
    {
        int start = threadIdx * chunkSize;
        int end = (threadIdx == numThreads - 1) ? numRays : start + chunkSize;
        threads.emplace_back([&, start, end]() -> void {
            Ray ray;
            ray.origin = lightPos;
            for (int i = start; i < end; ++i)
            {
                auto angle = (2.0F * static_cast<float>(M_PI) * i) / numRays;
                ray.direction = {std::cos(angle), std::sin(angle)};
                results[i] = scene.closestIntersection(ray);
            }
        });
    }
    for (auto &th : threads)
        th.join();
}
