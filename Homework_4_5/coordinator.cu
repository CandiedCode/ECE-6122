// ============================================================================
// coordinator.cu - (students complete the TODO sections)
// ECE 4122/6122  Homework 4 & 5 -- Distributed CUDA Ray Tracer
//
// Usage:  ./build/coordinator <image_width> <image_height> <tile_size> [port]
//   e.g.  ./build/coordinator 800 600 64
//
// Lifecycle:
//   1. Bind a non-blocking UDP socket and wait REGISTRATION_WAIT_MS for workers.
//   2. Build the full tile queue (one PktWorkOrder per tile, row-major).
//   3. Open an SFML window sized to the image.
//   4. Main loop (repeat until all tiles are done):
//        a. Poll SFML window events.
//        b. Dispatch pending tiles to idle workers.
//        c. Check for timed-out tiles and re-queue them.
//        d. Receive PktTileResult datagrams; copy pixels into sf::Image.
//        e. Upload the updated sf::Image to the sf::Texture and draw.
//   5. Broadcast PktDone to all workers; keep window open until user closes it.
// ============================================================================
#include "packets.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

// ── Tunables (provided) ───────────────────────────────────────────────────────
constexpr int REGISTRATION_WAIT_MS = 15000; // registration window length
constexpr int TILE_TIMEOUT_MS = 3000;       // ms before a tile is re-dispatched
constexpr int UDP_RECV_BUF = 65507;         // max UDP payload size

// ── Worker state (provided) ───────────────────────────────────────────────────
struct WorkerInfo
{
    sf::IpAddress ip;
    unsigned short port;
    bool idle = true;                                    // true  -> available for a new tile
    uint32_t current_tid = 0;                            // tile currently assigned to this worker
    std::chrono::steady_clock::time_point dispatch_time; // when the tile was sent
};

// ── Tile tracking (provided) ──────────────────────────────────────────────────
struct TileInfo
{
    PktWorkOrder order{}; // the work order packet for this tile
    bool done = false;
    bool in_flight = false;
};

// ── Timing helpers (provided) ─────────────────────────────────────────────────
using Clock = std::chrono::steady_clock;
using Ms = std::chrono::milliseconds;

static long long msElapsed(const Clock::time_point &t)
{
    return std::chrono::duration_cast<Ms>(Clock::now() - t).count();
}

// ============================================================================
// main
// ============================================================================
int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Usage: ./build/coordinator <image_width> <image_height> <tile_size> [port]\n";
        return 1;
    }

    const uint16_t image_w = static_cast<uint16_t>(std::stoi(argv[1]));
    const uint16_t image_h = static_cast<uint16_t>(std::stoi(argv[2]));
    const uint16_t tile_size = static_cast<uint16_t>(std::stoi(argv[3]));
    const unsigned short port = (argc >= 5) ? static_cast<unsigned short>(std::stoi(argv[4])) : 54000;

    // ── Scene description (provided) ─────────────────────────────────────
    // This struct is broadcast to every worker inside PktRegisterAck so they
    // all ray-trace the same scene from the same camera.
    SceneDesc scene;
    scene.image_w = image_w;
    scene.image_h = image_h;
    scene.tile_w = tile_size;
    scene.tile_h = tile_size;
    scene.cam_origin[0] = 0.f;
    scene.cam_origin[1] = 1.f;
    scene.cam_origin[2] = 5.f;
    scene.cam_lookat[0] = 0.f;
    scene.cam_lookat[1] = 0.f;
    scene.cam_lookat[2] = 0.f;
    scene.fov_deg = 60.f;

    // =========================================================================
    // TODO Part 1a  --  Bind coordinator socket (non-blocking)
    // =========================================================================
    // Create an sf::UdpSocket, bind it to 'port', then set it non-blocking.
    // Non-blocking is essential: the main loop must interleave dispatching,
    // receiving, and SFML window events without stalling on any one operation.
    //
    // Hint:
    //   sf::UdpSocket socket;
    //   socket.bind(port);
    //   socket.setBlocking(false);
    //
    sf::UdpSocket socket;
    // TODO: do we need to check other sf::Socket::Status values here?
    if (socket.bind(port) != sf::Socket::Done)
    {
        std::cerr << "[coord] Failed to bind socket to port " << port << "\n";
        return 1;
    }
    socket.setBlocking(false);

    std::cout << "[coord] Listening on port " << port << "\n"
              << "[coord] Waiting " << REGISTRATION_WAIT_MS << " ms for worker registrations...\n";

    // =========================================================================
    // TODO Part 1b  --  Registration phase
    // =========================================================================
    // Poll the non-blocking socket for REGISTRATION_WAIT_MS milliseconds.
    // For each incoming datagram:
    //   - Check buf[0] == PKT_REGISTER and received >= sizeof(PktRegister).
    //   - Deserialise into a PktRegister; extract sender IP and worker_port.
    //   - De-duplicate: only add the worker if (sender, reg.worker_port) is new.
    //   - Add a WorkerInfo to the workers vector.
    //   - Immediately send a PktRegisterAck containing the SceneDesc back to
    //     (sender, reg.worker_port).
    // Use sf::sleep(sf::milliseconds(10)) between polls to avoid busy-spinning.
    //
    // Hint:
    //   std::vector<WorkerInfo> workers;
    //   auto reg_start = Clock::now();
    //   while (msElapsed(reg_start) < REGISTRATION_WAIT_MS) {
    //       uint8_t buf[512]; std::size_t received;
    //       sf::IpAddress sender; unsigned short sport;
    //       if (socket.receive(buf,sizeof(buf),received,sender,sport)==sf::Socket::Done
    //           && buf[0]==PKT_REGISTER && received>=sizeof(PktRegister))
    //       {
    //           PktRegister reg;
    //           std::memcpy(&reg, buf, sizeof(reg));
    //           bool known = false;
    //           for (auto& w : workers)
    //               if (w.ip==sender && w.port==reg.worker_port) { known=true; break; }
    //           if (!known) {
    //               WorkerInfo wi; wi.ip=sender; wi.port=reg.worker_port;
    //               workers.push_back(wi);
    //               PktRegisterAck ack; ack.scene = scene;
    //               socket.send(&ack, sizeof(ack), sender, reg.worker_port);
    //               std::cout << "[coord] Worker: " << sender << ":" << reg.worker_port << "\n";
    //           }
    //       }
    //       sf::sleep(sf::milliseconds(10));
    //   }
    //
    std::vector<WorkerInfo> workers;
    auto reg_start = Clock::now();
    while (msElapsed(reg_start) < REGISTRATION_WAIT_MS)
    {
        uint8_t buf[512];
        std::size_t received;
        sf::IpAddress sender;
        unsigned short sport;
        if (socket.receive(buf, sizeof(buf), received, sender, sport) == sf::Socket::Done && buf[0] == PKT_REGISTER &&
            received >= sizeof(PktRegister))
        {
            PktRegister reg;
            std::memcpy(&reg, buf, sizeof(reg));
            bool known = false;
            for (auto &w : workers)
                if (w.ip == sender && w.port == reg.worker_port)
                {
                    known = true;
                    break;
                }
            if (!known)
            {
                WorkerInfo wi;
                wi.ip = sender;
                wi.port = reg.worker_port;
                workers.push_back(wi);
                PktRegisterAck ack;
                ack.scene = scene;
                socket.send(&ack, sizeof(ack), sender, reg.worker_port);
                std::cout << "[coord] Worker: " << sender << ":" << reg.worker_port << "\n";
            }
        }
        sf::sleep(sf::milliseconds(10));
    }

    if (workers.empty())
    {
        std::cerr << "[coord] No workers registered — exiting.\n";
        return 1;
    }
    std::cout << "[coord] " << workers.size() << " worker(s) registered.\n";

    // ── Build tile list and pending queue (provided) ──────────────────────
    // Tiles are generated in row-major order. Edge tiles are clamped so that
    // tile_w and tile_h reflect the actual pixel count at image boundaries.
    const int tiles_x = (image_w + tile_size - 1) / tile_size;
    const int tiles_y = (image_h + tile_size - 1) / tile_size;
    const int total_tiles = tiles_x * tiles_y;

    std::vector<TileInfo> tiles(total_tiles);
    std::queue<uint32_t> pending;

    for (int ty = 0; ty < tiles_y; ++ty)
    {
        for (int tx = 0; tx < tiles_x; ++tx)
        {
            uint32_t tid = static_cast<uint32_t>(ty * tiles_x + tx);
            TileInfo &ti = tiles[tid];
            ti.order.type = PKT_WORK_ORDER;
            ti.order.tile_id = tid;
            ti.order.x_offset = static_cast<uint16_t>(tx * tile_size);
            ti.order.y_offset = static_cast<uint16_t>(ty * tile_size);
            ti.order.tile_w = static_cast<uint16_t>(std::min((int)tile_size, (int)image_w - tx * (int)tile_size));
            ti.order.tile_h = static_cast<uint16_t>(std::min((int)tile_size, (int)image_h - ty * (int)tile_size));
            pending.push(tid);
        }
    }
    std::cout << "[coord] " << total_tiles << " tiles queued (" << tiles_x << " x " << tiles_y << ").\n";

    // ── SFML window + image setup (provided) ─────────────────────────────
    // sf::Image holds the pixel data on the CPU.
    // sf::Texture is the GPU-side copy (updated after each completed tile).
    // sf::Sprite draws the texture into the window.
    sf::RenderWindow window(sf::VideoMode(image_w, image_h), "Ray Tracer  \u2014  0 / " + std::to_string(total_tiles) + " tiles complete");
    window.setFramerateLimit(60);

    sf::Image full_image;
    full_image.create(image_w, image_h, sf::Color(18, 18, 18));

    sf::Texture texture;
    texture.create(image_w, image_h);
    texture.update(full_image);

    sf::Sprite sprite(texture);

    // Large receive buffer -- a 64×64 RGBA tile is ~16 KB; 65507 is safe.
    std::vector<uint8_t> recv_buf(UDP_RECV_BUF);

    int tiles_done = 0;
    auto render_start = Clock::now();

    // ── Main event / dispatch / receive loop ──────────────────────────────
    while (tiles_done < total_tiles)
    {

        // — SFML window events (provided) —
        sf::Event ev;
        while (window.pollEvent(ev))
            if (ev.type == sf::Event::Closed)
            {
                window.close();
                goto shutdown;
            }

        // =====================================================================
        // TODO Part 3a  --  Dispatch pending tiles to idle workers
        // =====================================================================
        // Iterate the workers vector. For each worker where w.idle == true:
        //   1. Skip finished tiles at the front of 'pending' (tiles[front].done).
        //   2. Pop the next tile_id from pending.
        //   3. Mark the worker busy: w.idle=false, w.current_tid=tid,
        //      w.dispatch_time=Clock::now(), tiles[tid].in_flight=true.
        //   4. Send tiles[tid].order to (w.ip, w.port) with socket.send().
        //
        // Hint:
        //   for (auto& w : workers) {
        //       if (!w.idle || pending.empty()) continue;
        //       while (!pending.empty() && tiles[pending.front()].done) pending.pop();
        //       if (pending.empty()) break;
        //       uint32_t tid = pending.front(); pending.pop();
        //       w.idle=false; w.current_tid=tid; w.dispatch_time=Clock::now();
        //       tiles[tid].in_flight=true;
        //       socket.send(&tiles[tid].order, sizeof(PktWorkOrder), w.ip, w.port);
        //   }
        //
        for (auto &w : workers)
        {
            if (!w.idle || pending.empty())
                continue;
            while (!pending.empty() && tiles[pending.front()].done)
            {
                pending.pop();
            }
            if (pending.empty())
            {
                break;
            }
            uint32_t tid = pending.front();
            pending.pop();
            w.idle = false;
            w.current_tid = tid;
            w.dispatch_time = Clock::now();
            tiles[tid].in_flight = true;
            socket.send(&tiles[tid].order, sizeof(PktWorkOrder), w.ip, w.port);
        }

        // =====================================================================
        // TODO Part 3b  --  Timeout check: re-queue stalled tiles
        // =====================================================================
        // For each non-idle worker, check if msElapsed(w.dispatch_time) > TILE_TIMEOUT_MS.
        // If so, and the tile isn't already done:
        //   - Print a timeout warning.
        //   - Set tiles[tid].in_flight = false and push tid back onto pending.
        // Either way, set w.idle = true so the worker can receive a new tile.
        //
        // Hint:
        //   for (auto& w : workers) {
        //       if (w.idle) continue;
        //       if (msElapsed(w.dispatch_time) > TILE_TIMEOUT_MS) {
        //           uint32_t tid = w.current_tid;
        //           if (!tiles[tid].done) {
        //               std::cout << "[coord] Timeout tile " << tid << " -- re-queuing\n";
        //               tiles[tid].in_flight = false;
        //               pending.push(tid);
        //           }
        //           w.idle = true;
        //       }
        //   }
        //
        for (auto &w : workers)
        {
            if (w.idle)
            {
                continue;
            }
            if (msElapsed(w.dispatch_time) > TILE_TIMEOUT_MS)
            {
                uint32_t tid = w.current_tid;
                if (!tiles[tid].done)
                {
                    std::cout << "[coord] Timeout tile " << tid << " -- re-queuing\n";
                    tiles[tid].in_flight = false;
                    pending.push(tid);
                }
                w.idle = true;
            }
        }

        // =====================================================================
        // TODO Part 3c / 4a  --  Receive PktTileResult and assemble image
        // =====================================================================
        // Drain all available datagrams from the non-blocking socket (loop until
        // socket.receive() returns something other than sf::Socket::Done).
        //
        // For each received datagram:
        //   A. Late registration:
        //      If buf[0]==PKT_REGISTER, handle it like the registration phase
        //      above (de-duplicate, send PktRegisterAck) then continue.
        //
        //   B. Tile result:
        //      If buf[0] != PKT_TILE_RESULT or received < sizeof(PktTileResult),
        //      skip (continue).
        //      Deserialise the PktTileResult header (hdr).
        //      Guard: if hdr.tile_id >= total_tiles or tiles[hdr.tile_id].done, skip.
        //      Verify payload: received >= sizeof(PktTileResult) + hdr.tile_w*hdr.tile_h*4.
        //
        //      Copy pixels into full_image using sf::Image::setPixel:
        //        const uint8_t* pixels = recv_buf.data() + sizeof(PktTileResult);
        //        for (py in [0, hdr.tile_h)):
        //          for (px in [0, hdr.tile_w)):
        //            int i = (py * hdr.tile_w + px) * 4;
        //            full_image.setPixel(ti.order.x_offset + px,
        //                                ti.order.y_offset + py,
        //                                sf::Color(pixels[i], pixels[i+1],
        //                                          pixels[i+2], pixels[i+3]));
        //
        //      Mark tile done, increment tiles_done.
        //      Mark the sending worker idle (match by sender IP + sport).
        //      Upload: texture.update(full_image);
        //      Update window title: "Ray Tracer -- X / Y tiles complete"
        //
        std::size_t received;
        sf::IpAddress sender;
        unsigned short sport;
        while (socket.receive(recv_buf.data(), recv_buf.size(), received, sender, sport) == sf::Socket::Done)
        {
            // Late registration
            if (recv_buf[0] == PKT_REGISTER && received >= sizeof(PktRegister))
            {
                PktRegister reg;
                std::memcpy(&reg, recv_buf.data(), sizeof(reg));
                bool known = false;
                for (auto &w : workers)
                    if (w.ip == sender && w.port == reg.worker_port)
                    {
                        known = true;
                        break;
                    }
                if (!known)
                {
                    WorkerInfo wi;
                    wi.ip = sender;
                    wi.port = reg.worker_port;
                    workers.push_back(wi);
                    PktRegisterAck ack;
                    ack.scene = scene;
                    socket.send(&ack, sizeof(ack), sender, reg.worker_port);
                    std::cout << "[coord] Late worker: " << sender << ":" << reg.worker_port << "\n";
                }
                continue;
            }

            // Tile result
            if (recv_buf[0] != PKT_TILE_RESULT || received < sizeof(PktTileResult))
                continue;

            PktTileResult hdr;
            std::memcpy(&hdr, recv_buf.data(), sizeof(hdr));

            if (hdr.tile_id >= (uint32_t)total_tiles || tiles[hdr.tile_id].done)
                continue;

            const std::size_t pixel_bytes = (std::size_t)hdr.tile_w * hdr.tile_h * 4;
            if (received < sizeof(PktTileResult) + pixel_bytes)
                continue;

            // Copy pixels into full_image
            const uint8_t *pixels = recv_buf.data() + sizeof(PktTileResult);
            TileInfo &ti = tiles[hdr.tile_id];
            for (uint16_t py = 0; py < hdr.tile_h; ++py)
            {
                for (uint16_t px = 0; px < hdr.tile_w; ++px)
                {
                    int i = (py * hdr.tile_w + px) * 4;
                    full_image.setPixel(ti.order.x_offset + px, ti.order.y_offset + py,
                                        sf::Color(pixels[i], pixels[i + 1], pixels[i + 2], pixels[i + 3]));
                }
            }

            // Mark tile done
            tiles[hdr.tile_id].done = true;
            tiles[hdr.tile_id].in_flight = false;
            ++tiles_done;

            // Mark sending worker idle
            for (auto &w : workers)
            {
                if (w.ip == sender && w.port == sport)
                {
                    w.idle = true;
                    break;
                }
            }

            // Update texture and window title
            texture.update(full_image);
            window.setTitle("Ray Tracer  \u2014  " + std::to_string(tiles_done) + " / " + std::to_string(total_tiles) + " tiles complete");
        }

        // — Draw frame (provided) —
        window.clear(sf::Color(18, 18, 18));
        window.draw(sprite);
        window.display();
    }

    // ── Render complete (provided) ────────────────────────────────────────
    {
        long long ms = msElapsed(render_start);
        std::cout << "[coord] Render complete in " << ms / 1000 << "." << (ms % 1000) / 10 << " s.\n";
    }

shutdown:
    // =====================================================================
    // TODO Part 1c  --  Broadcast PktDone to all workers
    // =====================================================================
    // Create a PktDone and socket.send() it to every worker.
    //
    // Hint:
    //   PktDone done_pkt;
    //   for (auto& w : workers)
    //       socket.send(&done_pkt, sizeof(done_pkt), w.ip, w.port);
    //
    PktDone done_pkt;
    for (auto &w : workers)
        socket.send(&done_pkt, sizeof(done_pkt), w.ip, w.port);

    std::cout << "[coord] PKT_DONE broadcast. Close the window to exit.\n";

    // Keep window open until the user closes it (provided).
    while (window.isOpen())
    {
        sf::Event ev;
        while (window.pollEvent(ev))
            if (ev.type == sf::Event::Closed)
                window.close();
        window.clear(sf::Color(18, 18, 18));
        window.draw(sprite);
        window.display();
    }

    return 0;
}
