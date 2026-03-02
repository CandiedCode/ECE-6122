/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-02-13
 * Description:  Homework 2: Ray Tracing Visualization with Multiple Rendering Modes
 *
 *
 * @file RayTracer.h
 * @brief Ray tracing header file. Contains the RayTracer class with methods for casting rays in single-threaded, OpenMP, and std::thread
 * modes.  These functions were provided as part of Homework 2 artifacts.
 */

#ifndef HOMEWORK_2_RAYTRACER_H_
#define HOMEWORK_2_RAYTRACER_H_

#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

// Forward declarations
struct HitResult;
class Scene;

#include "Geometry.h"
#include "Scene.h"

class RayTracer
{
  private:
    static constexpr float TWO_PI = 2.0F * static_cast<float>(M_PI);

  public:
    /** @brief Cast rays from a light source in a single thread
     *  @param lightPos The position of the light source
     *  @param numRays The number of rays to cast
     *  @param scene The scene to trace rays in
     *  @param results Vector to store HitResult for each ray
     */
    static auto castRaysSingleThreaded(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results)
        -> void;

    /** @brief Cast rays from a light source using OpenMP parallelization
     *  @param lightPos The position of the light source
     *  @param numRays The number of rays to cast
     *  @param scene The scene to trace rays in
     *  @param results Vector to store HitResult for each ray
     *  @param numThreads Number of threads to use
     */
    static auto castRaysOpenMP(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results,
                               int numThreads) -> void;

    /** @brief Cast rays from a light source using std::thread parallelization
     *  @param lightPos The position of the light source
     *  @param numRays The number of rays to cast
     *  @param scene The scene to trace rays in
     *  @param results Vector to store HitResult for each ray
     *  @param numThreads Number of threads to use
     */
    static auto castRaysStdThread(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results,
                                  int numThreads) -> void;
};

#endif // HOMEWORK_2_RAYTRACER_H_
