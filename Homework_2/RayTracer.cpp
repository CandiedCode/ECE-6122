#include "RayTracer.h"
#include "Scene.h"
#include <algorithm>
#include <cmath>
#include <omp.h>
#include <thread>
#include <vector>

auto RayTracer::castRaysSingleThreaded(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results)
    -> void
{
    results.resize(numRays);
    for (int i = 0; i < numRays; ++i)
    {
        float angle = static_cast<float>((2.0f * M_PI * i) / numRays);
        Ray ray;
        ray.origin = lightPos;
        ray.direction = {std::cos(angle), std::sin(angle)};
        results[i] = scene.closestIntersection(ray);
    }
}

auto RayTracer::castRaysOpenMP(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results,
                               int numThreads) -> void
{
    results.resize(numRays);
    omp_set_num_threads(numThreads);
#pragma omp parallel for schedule(static)
    for (int i = 0; i < numRays; ++i)
    {
        float angle = static_cast<float>((2.0f * M_PI * i) / numRays);
        Ray ray;
        ray.origin = lightPos;
        ray.direction = {std::cos(angle), std::sin(angle)};
        results[i] = scene.closestIntersection(ray);
    }
}

auto RayTracer::castRaysStdThread(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results,
                                  int numThreads) -> void
{
    results.resize(numRays);
    std::vector<std::thread> threads;
    int chunkSize = numRays / numThreads;
    for (int t = 0; t < numThreads; ++t)
    {
        int start = t * chunkSize;
        int end = (t == numThreads - 1) ? numRays : start + chunkSize;
        threads.emplace_back([&, start, end]() {
            for (int i = start; i < end; ++i)
            {
                float angle = (2.0f * M_PI * i) / numRays;
                Ray ray;
                ray.origin = lightPos;
                ray.direction = {std::cos(angle), std::sin(angle)};
                results[i] = scene.closestIntersection(ray);
            }
        });
    }
    for (auto &th : threads)
        th.join();
}
