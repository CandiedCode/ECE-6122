#include "RayTracer.h"
#include "Scene.h"
#include <algorithm>
#include <cmath>
#include <omp.h>
#include <thread>
#include <vector>

/** @brief Cast rays from a light source in a single thread
 * @param lightPos The position of the light source
 * @param numRays The number of rays to cast
 * @param scene The scene to trace rays in
 * @param results Vector to store HitResult for each ray
 */
void RayTracer::castRaysSingleThreaded(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results)
{
    results.resize(numRays);
    for (int i = 0; i < numRays; ++i)
    {
        float angle = (2.0f * M_PI * i) / numRays;
        Ray ray;
        ray.origin = lightPos;
        ray.direction = {std::cos(angle), std::sin(angle)};
        results[i] = scene.closestIntersection(ray);
    }
}

/** @brief Cast rays from a light source using OpenMP parallelization
 * @param lightPos The position of the light source
 * @param numRays The number of rays to cast
 * @param scene The scene to trace rays in
 * @param results Vector to store HitResult for each ray
 * @param numThreads Number of threads to use
 */
void RayTracer::castRaysOpenMP(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results,
                               int numThreads)
{
    results.resize(numRays);
    omp_set_num_threads(numThreads);
#pragma omp parallel for schedule(static)
    for (int i = 0; i < numRays; ++i)
    {
        float angle = (2.0f * M_PI * i) / numRays;
        Ray ray;
        ray.origin = lightPos;
        ray.direction = {std::cos(angle), std::sin(angle)};
        results[i] = scene.closestIntersection(ray);
    }
}

/** @brief Cast rays from a light source using std::thread parallelization
 * @param lightPos The position of the light source
 * @param numRays The number of rays to cast
 * @param scene The scene to trace rays in
 * @param results Vector to store HitResult for each ray
 * @param numThreads Number of threads to use
 */
void RayTracer::castRaysStdThread(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results,
                                  int numThreads)
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
