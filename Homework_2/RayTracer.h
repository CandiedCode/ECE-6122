#ifndef HOMEWORK_2_RAYTRACER_H_
#define HOMEWORK_2_RAYTRACER_H_

#include <SFML/Graphics.hpp>
#include <vector>

// Forward declarations
struct HitResult;
class Scene;

#include "Geometry.h"
#include "Scene.h"

class RayTracer
{
  public:
    /** @brief Cast rays from a light source in a single thread
     *  @param lightPos The position of the light source
     *  @param numRays The number of rays to cast
     *  @param scene The scene to trace rays in
     *  @param results Vector to store HitResult for each ray
     */
    static void castRaysSingleThreaded(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results);

    /** @brief Cast rays from a light source using OpenMP parallelization
     *  @param lightPos The position of the light source
     *  @param numRays The number of rays to cast
     *  @param scene The scene to trace rays in
     *  @param results Vector to store HitResult for each ray
     *  @param numThreads Number of threads to use
     */
    static void castRaysOpenMP(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results,
                               int numThreads);

    /** @brief Cast rays from a light source using std::thread parallelization
     *  @param lightPos The position of the light source
     *  @param numRays The number of rays to cast
     *  @param scene The scene to trace rays in
     *  @param results Vector to store HitResult for each ray
     *  @param numThreads Number of threads to use
     */
    static void castRaysStdThread(const sf::Vector2f &lightPos, int numRays, const Scene &scene, std::vector<HitResult> &results,
                                  int numThreads);
};

#endif // HOMEWORK_2_RAYTRACER_H_
