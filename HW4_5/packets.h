// ============================================================================
// packets.h  --  Shared UDP packet definitions for the distributed ray tracer
// ECE 4122 / 6122  Homework 5
// ============================================================================
#pragma once
#include <cstdint>
#include <cstring>

// ── Packet type discriminators ───────────────────────────────────────────────
constexpr uint8_t PKT_REGISTER = 0x01;
constexpr uint8_t PKT_REGISTER_ACK = 0x02;
constexpr uint8_t PKT_WORK_ORDER = 0x03;
constexpr uint8_t PKT_TILE_RESULT = 0x04;
constexpr uint8_t PKT_DONE = 0x05;

// ── Scene description (embedded in PktRegisterAck) ───────────────────────────
struct SceneDesc
{
    uint16_t image_w;    // full image width  (px)
    uint16_t image_h;    // full image height (px)
    uint16_t tile_w;     // nominal tile width  -- actual may be smaller at edges
    uint16_t tile_h;     // nominal tile height
    float cam_origin[3]; // camera position  (x, y, z)
    float cam_lookat[3]; // look-at point
    float fov_deg;       // vertical field-of-view (degrees)
};

// ── Worker -> Coordinator  :  announce presence ──────────────────────────────
struct PktRegister
{
    uint8_t type = PKT_REGISTER;
    uint16_t worker_port; // UDP port the worker is listening on
};

// ── Coordinator -> Worker  :  confirm + deliver scene ────────────────────────
struct PktRegisterAck
{
    uint8_t type = PKT_REGISTER_ACK;
    SceneDesc scene;
};

// ── Coordinator -> Worker  :  one tile assignment ────────────────────────────
struct PktWorkOrder
{
    uint8_t type = PKT_WORK_ORDER;
    uint32_t tile_id;  // unique tile index (row-major)
    uint16_t x_offset; // left edge of tile in the full image  (px)
    uint16_t y_offset; // top  edge of tile in the full image  (px)
    uint16_t tile_w;   // actual tile width  (may be < nominal at right edge)
    uint16_t tile_h;   // actual tile height (may be < nominal at bottom edge)
};

// ── Worker -> Coordinator  :  completed tile ─────────────────────────────────
// Pixel data (RGBA, row-major within tile) is appended immediately after this
// header in the same UDP datagram.
struct PktTileResult
{
    uint8_t type = PKT_TILE_RESULT;
    uint32_t tile_id;
    uint16_t tile_w;
    uint16_t tile_h;
    // uint8_t pixels[tile_w * tile_h * 4]  follows in payload
};

// ── Coordinator -> All Workers  :  rendering finished ────────────────────────
struct PktDone
{
    uint8_t type = PKT_DONE;
};
