// ============================================================================
// worker.cu - (students complete the TODO sections)
// ECE 4122/6122 Homework 4 & 5 -- Distributed CUDA Ray Tracer
//
// Usage:  ./build/worker <coordinator_ip> <coordinator_port>
//   e.g.  ./build/worker 127.0.0.1 54000
//
// Lifecycle:
//   1. Bind a UDP socket to an ephemeral port.
//   2. Send PktRegister to the coordinator; wait for PktRegisterAck (SceneDesc).
//   3. Build camera basis vectors from the received scene parameters.
//   4. Upload the hardcoded scene (spheres + lights) to GPU constant memory.
//   5. Loop:
//        a. Receive PktWorkOrder from the coordinator.
//        b. Launch renderTile CUDA kernel.
//        c. cudaMemcpy pixels device -> host.
//        d. Send PktTileResult back to the coordinator.
//      Until PktDone is received.
// ============================================================================
#include "packets.h"
#include <SFML/Network.hpp>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cuda_runtime.h>
#include <iostream>
#include <vector>

const float PI = 3.14159265358979f;
const float ALPHA = 255.F;

// ── CUDA error-checking macro (provided) ─────────────────────────────────────
#define CUDA_CHECK(call)                                                                                                                   \
    do                                                                                                                                     \
    {                                                                                                                                      \
        cudaError_t _err = (call);                                                                                                         \
        if (_err != cudaSuccess)                                                                                                           \
        {                                                                                                                                  \
            fprintf(stderr, "CUDA error at %s:%d  %s\n", __FILE__, __LINE__, cudaGetErrorString(_err));                                    \
            exit(EXIT_FAILURE);                                                                                                            \
        }                                                                                                                                  \
    } while (0)

// ============================================================================
// Math types (provided -- do not add constructors; see comment below)
// ============================================================================
// IMPORTANT: Vec3 must remain a trivially constructible aggregate.
// CUDA forbids runtime (dynamic) initialisation of __constant__ variables.
// Because Sphere and Light embed Vec3, any user-defined constructor on Vec3
// would make those types non-trivially constructible and trigger the error:
//   "dynamic initialization is not supported for a __constant__ variable"
// Use brace-aggregate syntax everywhere:  Vec3 v = {x, y, z};  -- never Vec3(x,y,z).
struct Vec3
{
    float x, y, z;

    __host__ __device__ Vec3 operator+(const Vec3 &b) const
    {
        return {x + b.x, y + b.y, z + b.z};
    }
    __host__ __device__ Vec3 operator-(const Vec3 &b) const
    {
        return {x - b.x, y - b.y, z - b.z};
    }
    __host__ __device__ Vec3 operator*(float t) const
    {
        return {x * t, y * t, z * t};
    }
    __host__ __device__ Vec3 operator*(const Vec3 &b) const
    {
        return {x * b.x, y * b.y, z * b.z};
    }
    __host__ __device__ Vec3 operator+(float t) const
    {
        return {x + t, y + t, z + t};
    }
    __host__ __device__ Vec3 &operator+=(const Vec3 &b)
    {
        x += b.x;
        y += b.y;
        z += b.z;
        return *this;
    }
    __host__ __device__ float dot(const Vec3 &b) const
    {
        return x * b.x + y * b.y + z * b.z;
    }
    __host__ __device__ float length() const
    {
        return sqrtf(dot(*this));
    }
    __host__ __device__ Vec3 normalize() const
    {
        float l = length();
        return {x / l, y / l, z / l};
    }
    __host__ __device__ Vec3 clamp01() const
    {
        return {fminf(1.F, fmaxf(0.F, x)), fminf(1.F, fmaxf(0.F, y)), fminf(1.F, fmaxf(0.F, z))};
    }
};

// Host-only cross product (used for camera basis construction below)
static Vec3 cross(const Vec3 &a, const Vec3 &b)
{
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

// ============================================================================
// Scene types (provided -- do not modify)
// ============================================================================
struct Sphere
{
    Vec3 center;
    float radius;
    Vec3 color;         // diffuse albedo  (0..1 per channel)
    float shininess;    // Phong exponent
    float reflectivity; // reserved for graduate extension
};

struct Light
{
    Vec3 pos;
    Vec3 color;
};

#define NUM_SPHERES 7
#define NUM_LIGHTS 2

// Scene data lives in GPU constant memory and is uploaded once in main().
// Every thread in every warp reads the same constant-memory address per
// instruction, so the constant cache handles it in a single broadcast -- zero
// extra global-memory traffic compared with passing arrays as kernel arguments.
__constant__ Sphere d_spheres[NUM_SPHERES];
__constant__ Light d_lights[NUM_LIGHTS];

// ============================================================================
// TODO Part 2b  --  Ray-sphere intersection
// ============================================================================
// Returns true if the ray (origin ro, direction rd) hits sphere s, and sets
// t_hit to the distance along rd to the nearest intersection point.
//
// Use the half-b quadratic formula to find t:
//
//   oc    = ro - s.center
//   b     = oc.dot(rd)          // note: this is the HALF coefficient
//   c     = oc.dot(oc) - s.radius * s.radius
//   disc  = b*b - c             // discriminant (using half-b, so no factor of 4)
//
//   If disc < 0  --> no intersection, return false.
//
//   sq = sqrtf(disc)
//   t0 = -b - sq               // near root
//   t1 = -b + sq               // far  root
//
//   If both roots are behind the ray (t1 < 1e-4f), return false.
//   Otherwise set t_hit = t0 if t0 >= 1e-4f, else t1  (prefer the nearer root
//   that is in front of the ray origin).
//
__device__ bool intersect(const Vec3 &ro, const Vec3 &rd, const Sphere &s, float &t_hit)
{
    Vec3 origin_to_center = ro - s.center;
    float b = origin_to_center.dot(rd); // half coefficient
    float c = origin_to_center.dot(origin_to_center) - s.radius * s.radius;
    float discriminant = b * b - c;

    if (discriminant < 0.F)
        return false;

    float sq = sqrtf(discriminant);
    float t0 = -b - sq; // near root
    float t1 = -b + sq; // far root

    float atol = 1e-4f; // absolute tolerance to avoid self-intersection

    // Both roots behind ray
    // t0 ≤ t1, so if t1 < atol, then t0 < atol too.
    if (t1 < atol)
    {
        return false;
    }

    // Prefer nearer root in front of ray origin
    // If t0 is in front of ray origin, use it; otherwise, use t1.
    if (t0 >= atol)
    {
        t_hit = t0;
    }
    // t1 >= atol,  t0 < atol, thus t0 is behind ray origin but t1 is in front.
    else
    {
        t_hit = t1;
    }
    return true;
}

// Shadow ray - iterates intersect() over all spheres.
// Returns true if anything blocks the path from 'origin' to distance max_t.
__device__ bool shadowRay(const Vec3 &origin, const Vec3 &dir, float max_t)
{
    for (int i = 0; i < NUM_SPHERES; ++i)
    {
        float t;
        if (intersect(origin, dir, d_spheres[i], t) && t < max_t - 1e-3F)
            return true;
    }
    return false;
}

// ============================================================================
// TODO Part 2c  --  Phong shading
// ============================================================================
// Compute the lit colour at a surface point using the Phong illumination model.
// Parameters:
//   hit_pt   -- world-space hit position
//   normal   -- outward unit surface normal at hit_pt
//   view_dir -- unit vector pointing FROM hit_pt TOWARD the camera
//   s        -- the sphere that was hit (carries color and shininess)
//
// Algorithm for each light source d_lights[i]:
//   1. Ambient term (add once, outside the light loop):
//        result = s.color * 0.12f
//
//   2. Compute the vector from hit_pt toward the light:
//        to_light = d_lights[i].pos - hit_pt
//        dist     = to_light.length()
//        L        = to_light * (1.F / dist)   // normalise
//
//   3. Shadow test -- call shadowRay(hit_pt, L, dist).
//      If the path is blocked, skip steps 4 and 5 for this light.
//
//   4. Diffuse (Lambertian):
//        diff   = max(0, normal.dot(L))
//        result += s.color * d_lights[i].color * diff
//
//   5. Specular (Blinn-Phong half-vector):
//        H    = (L + view_dir).normalize()
//        spec = pow(max(0, normal.dot(H)), s.shininess)
//        result += d_lights[i].color * spec * 0.6f
//
//   Return result.clamp01() to keep colours in [0,1].
//
__device__ Vec3 shade(const Vec3 &hit_pt, const Vec3 &normal, const Vec3 &view_dir, const Sphere &s)
{
    // ambient term
    // start with 12% of the sphere's base color.
    Vec3 result = s.color * 0.12F;

    // for each light sources
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        // Compute vector from hit point toward the light
        Vec3 to_light = d_lights[i].pos - hit_pt;
        float dist = to_light.length();
        Vec3 L = to_light * (1.F / dist); // normalize

        // shadow test
        if (shadowRay(hit_pt, L, dist))
        {
            continue; // light is blocked, skip this light
        }

        // diffuse - Lambertian
        // 0 -> dark, 1 -> bright
        float diff = fmaxf(0.F, normal.dot(L));
        result += s.color * d_lights[i].color * diff;

        // specular - Blinn-Phong
        Vec3 H = (L + view_dir).normalize();
        float spec = pow(fmaxf(0.F, normal.dot(H)), s.shininess);
        result += d_lights[i].color * spec * 0.6F;
    }

    return result.clamp01();
}

// ============================================================================
// TODO Part 2e (bonus)  --  Sky background gradient
// ============================================================================
// If a primary ray misses all spheres, return a sky colour instead of black.
// Map the ray's normalised y-component from [-1, +1] to [0, 1]:
//
//   t = 0.5f * (rd.normalize().y + 1.F)   // 0 at horizon, 1 at zenith
//
// Then linearly interpolate between a warm horizon colour and a cool zenith:
//
//   horizon = {0.95f, 0.80f, 0.55f}
//   zenith  = {0.08f, 0.18f, 0.48f}
//   return  horizon * (1.F - t) + zenith * t
//
__device__ Vec3 skyColor(const Vec3 &rd)
{
    // Map normalized ray y-component from [-1, 1] to [0, 1]
    float t = 0.5f * (rd.normalize().y + 1.F);

    // Interpolate between warm horizon and cool zenith
    Vec3 horizon = {0.95f, 0.80f, 0.55f};
    Vec3 zenith = {0.08f, 0.18f, 0.48f};
    return horizon * (1.F - t) + zenith * t;
}

// ============================================================================
// TODO Part 2d  --  Tile rendering kernel
// ============================================================================
// Launch configuration: 2-D blocks of 16x16 threads.
// Each thread renders exactly ONE pixel of the tile.
//
// Thread-to-pixel mapping:
//   px (tile-local x) = threadIdx.x + blockIdx.x * blockDim.x
//   py (tile-local y) = threadIdx.y + blockIdx.y * blockDim.y
//   Guard: if (px >= tile_w || py >= tile_h) return;
//
// Image pixel coordinates:
//   img_x = px + x_offset
//   img_y = py + y_offset
//
// Compute NDC (Normalised Device Coordinates) for this pixel:
//   aspect = (float)image_w / (float)image_h
//   half_h = tanf(fov_deg * 0.5f * PI / 180.F)   // PI = 3.14159265358979f
//   half_w = half_h * aspect
//
//   u =   ((img_x + 0.5f) / image_w) * 2.F - 1.F   // [-1, +1], left to right
//   v = -(((img_y + 0.5f) / image_h) * 2.F - 1.F)  // [-1, +1], bottom to top
//                                                    // (flip y: row 0 is top)
//
// Primary ray direction:
//   rd = (cam_forward + cam_right * (u * half_w) + cam_up * (v * half_h)).normalize()
//
// Trace the ray: iterate d_spheres[0..NUM_SPHERES-1], call intersect().
// Keep track of the closest hit (smallest t).
//
// If a sphere was hit:
//   hit_pt = cam_origin + rd * t_min
//   normal = (hit_pt - d_spheres[hit_idx].center).normalize()
//   view   = (cam_origin - hit_pt).normalize()
//   color  = shade(hit_pt, normal, view, d_spheres[hit_idx])
// Else:
//   color  = skyColor(rd)
//
// Write RGBA to d_pixels (alpha = 255):
//   int out_idx = (py * tile_w + px) * 4;
//   d_pixels[out_idx + 0] = (uint8_t)(color.x * 255.F);
//   d_pixels[out_idx + 1] = (uint8_t)(color.y * 255.F);
//   d_pixels[out_idx + 2] = (uint8_t)(color.z * 255.F);
//   d_pixels[out_idx + 3] = 255u;
//
__global__ void renderTile(uint8_t *d_pixels, // output: RGBA pixels, row-major within tile
                           uint16_t x_offset, // tile's left edge in the full image (px)
                           uint16_t y_offset, // tile's top  edge in the full image (px)
                           uint16_t tile_w,   // actual tile width  (may be < nominal at right edge)
                           uint16_t tile_h,   // actual tile height (may be < nominal at bottom edge)
                           uint16_t image_w,  // full image width  (px)
                           uint16_t image_h,  // full image height (px)
                           float fov_deg,     // vertical field-of-view (degrees)
                           Vec3 cam_origin,   // camera position
                           Vec3 cam_forward,  // unit vector: camera forward direction
                           Vec3 cam_right,    // unit vector: camera right direction
                           Vec3 cam_up        // unit vector: camera up direction
)
{
    // Thread-to-pixel mapping
    uint16_t pixel_x = threadIdx.x + blockIdx.x * blockDim.x;
    uint16_t pixel_y = threadIdx.y + blockIdx.y * blockDim.y;

    // Guard: check bounds
    if (pixel_x >= tile_w || pixel_y >= tile_h)
    {
        return;
    }

    // Image pixel coordinates
    uint16_t image_x = pixel_x + x_offset;
    uint16_t image_y = pixel_y + y_offset;

    // Compute NDC (Normalised Device Coordinates) for this pixel:
    float aspect = static_cast<float>(image_w) / static_cast<float>(image_h);
    float half_h = tanf(fov_deg * 0.5F * PI / 180.F);
    float half_w = half_h * aspect;

    // Compute normalized device coordinates (NDC)
    float u = ((image_x + 0.5F) / image_w) * 2.F - 1.F;    // [-1, +1], left to right
    float v = -(((image_y + 0.5F) / image_h) * 2.F - 1.F); // [-1, +1], bottom to top (flip y: row 0 is top)

    // Primary ray direction
    Vec3 rd = (cam_forward + cam_right * (u * half_w) + cam_up * (v * half_h)).normalize();

    // Trace the ray
    float t_min = 1e30F; // large number to represent "infinity"
    int hit_idx = -1;

    for (int i = 0; i < NUM_SPHERES; ++i)
    {
        float t;
        if (intersect(cam_origin, rd, d_spheres[i], t))
        {
            // If a sphere was hit, check if it's the closest hit so far
            if (t < t_min)
            {
                t_min = t;
                hit_idx = i;
            }
        }
    }

    // Compute final color
    Vec3 color;
    if (hit_idx >= 0)
    {
        // Sphere was hit
        Vec3 hit_pt = cam_origin + rd * t_min;
        Vec3 normal = (hit_pt - d_spheres[hit_idx].center).normalize();
        Vec3 view = (cam_origin - hit_pt).normalize();
        color = shade(hit_pt, normal, view, d_spheres[hit_idx]);
    }
    else
    {
        color = skyColor(rd);
    }

    // Write RGBA to d_pixels (alpha = 255)
    int out_idx = (pixel_y * tile_w + pixel_x) * 4;
    d_pixels[out_idx + 0] = (uint8_t)(color.x * ALPHA);
    d_pixels[out_idx + 1] = (uint8_t)(color.y * ALPHA);
    d_pixels[out_idx + 2] = (uint8_t)(color.z * ALPHA);
    d_pixels[out_idx + 3] = 255u;
}

// ============================================================================
// main
// ============================================================================
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: ./build/worker <coordinator_ip> <coordinator_port>\n";
        return 1;
    }

    sf::IpAddress coord_ip(argv[1]);
    unsigned short coord_port = static_cast<unsigned short>(std::stoi(argv[2]));

    // =========================================================================
    // TODO Part 1a  --  Bind UDP socket to an ephemeral port
    // =========================================================================
    // Create an sf::UdpSocket.
    // Bind it to sf::Socket::AnyPort so the OS assigns a free port.
    // Read the assigned port back with socket.getLocalPort().
    //
    // Hint:
    //   sf::UdpSocket socket;
    //   socket.bind(sf::Socket::AnyPort);
    //   unsigned short my_port = socket.getLocalPort();
    //
    sf::UdpSocket socket;
    socket.bind(sf::Socket::AnyPort);
    unsigned short my_port = socket.getLocalPort();

    std::cout << "[worker] Bound to port " << my_port << "\n";

    // =========================================================================
    // TODO Part 1b  --  Send PktRegister to the coordinator
    // =========================================================================
    // Fill a PktRegister with worker_port = my_port.
    // Send it to coord_ip:coord_port with socket.send().
    //
    // Hint:
    //   PktRegister reg;
    //   reg.worker_port = my_port;
    //   socket.send(&reg, sizeof(reg), coord_ip, coord_port);
    //
    PktRegister reg;
    reg.worker_port = my_port;
    socket.send(&reg, sizeof(reg), coord_ip, coord_port);

    std::cout << "[worker] Registration sent to " << coord_ip << ":" << coord_port << "\n";

    // =========================================================================
    // TODO Part 1c  --  Receive PktRegisterAck (blocking)
    // =========================================================================
    // Switch the socket to blocking mode: socket.setBlocking(true);
    // Loop calling socket.receive() until you get a datagram whose first byte
    // is PKT_REGISTER_ACK and whose size is >= sizeof(PktRegisterAck).
    // memcpy the datagram into a PktRegisterAck and store ack.scene.
    //
    // Hint:
    //   socket.setBlocking(true);
    //   SceneDesc scene{};
    //   uint8_t buf[512]; std::size_t received;
    //   sf::IpAddress sender; unsigned short sport;
    //   for (;;) {
    //       if (socket.receive(buf, sizeof(buf), received, sender, sport)
    //               == sf::Socket::Done
    //           && buf[0] == PKT_REGISTER_ACK
    //           && received >= sizeof(PktRegisterAck))
    //       {
    //           PktRegisterAck ack;
    //           std::memcpy(&ack, buf, sizeof(ack));
    //           scene = ack.scene;
    //           break;
    //       }
    //   }
    //
    socket.setBlocking(true);
    SceneDesc scene{};
    uint8_t buf[512];
    std::size_t received;
    sf::IpAddress sender;
    unsigned short sport;
    bool continue_loop = true;

    while (continue_loop)
    {
        if (socket.receive(buf, sizeof(buf), received, sender, sport) == sf::Socket::Done && buf[0] == PKT_REGISTER_ACK &&
            received >= sizeof(PktRegisterAck))
        {
            PktRegisterAck ack;
            std::memcpy(&ack, buf, sizeof(ack));
            scene = ack.scene;
            continue_loop = false;
        }
    }

    std::cout << "[worker] Scene received: " << scene.image_w << "x" << scene.image_h << "  tiles " << scene.tile_w << "x" << scene.tile_h
              << "  fov=" << scene.fov_deg << " deg\n";

    // ── Build camera basis vectors (provided) ─────────────────────────────
    // These are computed once on the host and passed to every kernel launch.
    // forward = normalise(lookat - origin)
    // right   = normalise(forward x world_up)
    // up      = normalise(right   x forward)   (re-orthogonalised)
    Vec3 cam_origin = {scene.cam_origin[0], scene.cam_origin[1], scene.cam_origin[2]};
    Vec3 cam_lookat = {scene.cam_lookat[0], scene.cam_lookat[1], scene.cam_lookat[2]};
    Vec3 world_up = {0.F, 1.F, 0.F};
    Vec3 cam_forward = (cam_lookat - cam_origin).normalize();
    Vec3 cam_right = cross(cam_forward, world_up).normalize();
    Vec3 cam_up_vec = cross(cam_right, cam_forward).normalize();

    // ── Upload scene geometry to GPU constant memory (provided) ──────────
    // Modify the sphere/light data below to change the scene.
    Sphere h_spheres[NUM_SPHERES] = {
        //  center                  radius  color (RGB 0-1)       shine  refl
        {{0.F, -100.5F, 0.F}, 100.F, {0.40F, 0.65F, 0.30F}, 8.F, 0.F},  // ground
        {{0.F, 0.0F, 0.F}, 0.5F, {0.85F, 0.15F, 0.15F}, 64.F, 0.F},     // red (centre)
        {{1.2F, 0.0F, -0.5F}, 0.5F, {0.15F, 0.40F, 0.90F}, 128.F, 0.F}, // blue (right)
        {{-1.2F, 0.0F, -0.5F}, 0.5F, {0.90F, 0.75F, 0.10F}, 32.F, 0.F}, // gold (left)
        {{0.F, 1.2F, -1.0F}, 0.5F, {0.60F, 0.10F, 0.85F}, 256.F, 0.F},  // purple (top)
        {{0.6F, -0.2F, 1.0F}, 0.3F, {0.10F, 0.80F, 0.60F}, 64.F, 0.F},  // teal (front-right)
        {{-0.6F, -0.2F, 1.0F}, 0.3F, {0.95F, 0.50F, 0.10F}, 64.F, 0.F}, // orange (front-left)
    };
    Light h_lights[NUM_LIGHTS] = {
        {{5.F, 8.F, 5.F}, {1.00F, 0.95F, 0.90F}},  // warm key light
        {{-3.F, 4.F, 2.F}, {0.40F, 0.50F, 0.80F}}, // cool fill light
    };

    CUDA_CHECK(cudaMemcpyToSymbol(d_spheres, h_spheres, sizeof(h_spheres)));
    CUDA_CHECK(cudaMemcpyToSymbol(d_lights, h_lights, sizeof(h_lights)));

    // ── Pre-size the host pixel buffer ────────────────────────────────────
    // Resized inside the loop if an edge tile turns out to be larger than
    // the nominal dimensions (shouldn't happen, but defensive).
    const std::size_t nom_pixels = (std::size_t)scene.tile_w * scene.tile_h * 4;
    std::vector<uint8_t> host_pixels(nom_pixels);

    // ── Work loop ─────────────────────────────────────────────────────────
    for (;;)
    {
        // =====================================================================
        // TODO Part 1d  --  Receive PktWorkOrder (or PktDone)
        // =====================================================================
        // Receive a datagram into a small stack buffer.
        // If buf[0] == PKT_DONE   --> print a message and break out of the loop.
        // If buf[0] != PKT_WORK_ORDER or received < sizeof(PktWorkOrder) --> continue.
        // Otherwise memcpy the buffer into a PktWorkOrder struct named 'order'.
        //
        // Hint:
        //   uint8_t buf[sizeof(PktWorkOrder) + 32];
        //   std::size_t received;
        //   sf::IpAddress sender; unsigned short sport;
        //   socket.receive(buf, sizeof(buf), received, sender, sport);
        //   if (buf[0] == PKT_DONE) { ...; break; }
        //   if (buf[0] != PKT_WORK_ORDER || received < sizeof(PktWorkOrder)) continue;
        //   PktWorkOrder order;
        //   std::memcpy(&order, buf, sizeof(order));
        //
        uint8_t buf[sizeof(PktWorkOrder) + 32];
        std::size_t received;
        sf::IpAddress sender;
        unsigned short sport;
        socket.receive(buf, sizeof(buf), received, sender, sport);
        if (buf[0] == PKT_DONE)
        {
            std::cout << "[worker] Received PKT_DONE, exiting\n";
            break;
        }
        if (buf[0] != PKT_WORK_ORDER || received < sizeof(PktWorkOrder))
        {
            continue;
        }
        PktWorkOrder order{};
        std::memcpy(&order, buf, sizeof(order));

        std::cout << "[worker] Tile " << order.tile_id << "  offset=(" << order.x_offset << "," << order.y_offset << ")"
                  << "  size=" << order.tile_w << "x" << order.tile_h << "\n";

        const std::size_t pixel_bytes = (std::size_t)order.tile_w * order.tile_h * 4;

        // =====================================================================
        // TODO Part 2d  --  Allocate device pixel buffer
        // =====================================================================
        // Use cudaMalloc to allocate pixel_bytes bytes on the device.
        // Wrap the call with CUDA_CHECK.
        //
        // Hint:
        //   uint8_t* d_pixels = nullptr;
        //   CUDA_CHECK(cudaMalloc(&d_pixels, pixel_bytes));
        //
        uint8_t *d_pixels = nullptr;
        CUDA_CHECK(cudaMalloc(&d_pixels, pixel_bytes));

        // =====================================================================
        // TODO Part 2e  --  Launch renderTile kernel
        // =====================================================================
        // Block size: dim3 block(16, 16)  -- 256 threads per block.
        // Grid size:  ceil(tile_w / 16) x ceil(tile_h / 16).
        //
        // Hint:
        //   dim3 block(16, 16);
        //   dim3 grid((order.tile_w + 15u) / 16u,
        //             (order.tile_h + 15u) / 16u);
        //   renderTile<<<grid, block>>>( d_pixels,
        //       order.x_offset, order.y_offset, order.tile_w, order.tile_h,
        //       scene.image_w,  scene.image_h,  scene.fov_deg,
        //       cam_origin, cam_forward, cam_right, cam_up_vec );
        //   CUDA_CHECK(cudaGetLastError());
        //   CUDA_CHECK(cudaDeviceSynchronize());
        //
        dim3 block(16, 16);
        dim3 grid((order.tile_w + 15u) / 16u, (order.tile_h + 15u) / 16u);
        renderTile<<<grid, block>>>(d_pixels, order.x_offset, order.y_offset, order.tile_w, order.tile_h, scene.image_w, scene.image_h,
                                    scene.fov_deg, cam_origin, cam_forward, cam_right, cam_up_vec);
        CUDA_CHECK(cudaGetLastError());
        CUDA_CHECK(cudaDeviceSynchronize());

        // =====================================================================
        // TODO Part 2f  --  Copy rendered pixels device -> host
        // =====================================================================
        // Resize host_pixels if needed, then cudaMemcpy DeviceToHost.
        // Free d_pixels with CUDA_CHECK(cudaFree(d_pixels)).
        //
        // Hint:
        //   if (host_pixels.size() < pixel_bytes) host_pixels.resize(pixel_bytes);
        //   CUDA_CHECK(cudaMemcpy(host_pixels.data(), d_pixels,
        //                         pixel_bytes, cudaMemcpyDeviceToHost));
        //   CUDA_CHECK(cudaFree(d_pixels));
        //
        if (host_pixels.size() < pixel_bytes)
        {
            host_pixels.resize(pixel_bytes);
        }
        CUDA_CHECK(cudaMemcpy(host_pixels.data(), d_pixels, pixel_bytes, cudaMemcpyDeviceToHost));
        CUDA_CHECK(cudaFree(d_pixels));

        // =====================================================================
        // TODO Part 1e  --  Send PktTileResult back to the coordinator
        // =====================================================================
        // Build a contiguous datagram:
        //   [ PktTileResult header ][ RGBA pixel bytes ]
        //
        // Hint:
        //   std::vector<uint8_t> pkt(sizeof(PktTileResult) + pixel_bytes);
        //   PktTileResult hdr;
        //   hdr.tile_id = order.tile_id;
        //   hdr.tile_w  = order.tile_w;
        //   hdr.tile_h  = order.tile_h;
        //   std::memcpy(pkt.data(),              &hdr,                sizeof(hdr));
        //   std::memcpy(pkt.data()+sizeof(hdr),  host_pixels.data(),  pixel_bytes);
        //   socket.send(pkt.data(), pkt.size(), coord_ip, coord_port);
        //
        std::vector<uint8_t> pkt(sizeof(PktTileResult) + pixel_bytes);
        PktTileResult hdr;
        hdr.tile_id = order.tile_id;
        hdr.tile_w = order.tile_w;
        hdr.tile_h = order.tile_h;
        std::memcpy(pkt.data(), &hdr, sizeof(hdr));
        std::memcpy(pkt.data() + sizeof(hdr), host_pixels.data(), pixel_bytes);
        socket.send(pkt.data(), pkt.size(), coord_ip, coord_port);

        std::cout << "[worker] Sent tile " << order.tile_id << "\n";
    }

    return 0;
}
