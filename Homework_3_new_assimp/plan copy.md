# Homework 3 — Robot Scene Plan

## Context
Homework 3 requires a 3D OpenGL scene using Assimp model loading, Phong lighting, FPS camera, and texturing. We're building a robot scene (not the farmstead template). The scaffolding code (Camera, Shader, Texture, Mesh, Model, shaders) is largely in place but has several bugs and incompletions. Scene objects in `Hw3.cpp` still reference non-existent farm assets.

---

## Asset Situation

**Current assets:**
- `batamax/batamax.obj` — OBJ format, no .mtl/textures (loads as untextured geometry)
- `robot.glb`, `cute_low_poly_friendly_robot.glb`, `repo_robot.glb`, `weathered_penguin-bot.glb`, `spot_robot_low_poly_model_with_rig.glb`

**Strategy: Test .glb with Assimp first (Step 2). Assimp 5.x natively supports .glb/.gltf.** If loading succeeds and textures appear → use them. If they fail → download OBJ alternatives.

Need 5+ distinct models total. If .glb works we have enough. If not, candidates for OBJ download: low-poly robots, sci-fi crates, antenna/beacon, platform.

---

## Work Breakdown (recommended order)

### Step 1 — Fix Existing Code Bugs

**`src/shader.h`** — Remove stray `#endif` (line ~21, no matching `#ifdef`)

**`src/model.cpp`** — Two fixes:
1. Add `aiProcess_CalcTangentSpace` to import flags (required by spec)
2. Add error checking after `importer.ReadFile()`:
   ```cpp
   if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
       throw std::runtime_error(importer.GetErrorString());
   ```

**`shaders/object.frag`** — Fix directional light direction in `CalcDirLight`:
```glsl
vec3 lightDir = normalize(-l.direction);  // was: normalize(l.direction)
```
(Without this, most upward-facing surfaces get zero diffuse from the sun.)

---

### Step 2 — Verify .glb Asset Loading

Build and run with one `.glb` model substituted into Hw3.cpp to confirm Assimp loads it. Check:
- Does the mesh appear?
- Do textures appear?
- Any crash or error output?

If .glb works → proceed with current assets.
If .glb fails → download 4 more OBJ robot models before Step 3.

```bash
mkdir -p build && cd build && cmake .. && make -j$(nproc) && ./bin/Hw3
```

---

### Step 3 — Add Ground Plane (inline in Hw3.cpp)

Build a flat quad inline in `main()` — no new class needed:
- 4 vertices on the XZ plane at y=0, spanning ±15 units
- Tile UVs at 10x10 repeats so a texture visibly tiles
- Load a concrete/dirt/metal-grate texture
- Push a `SceneObject` for it (or handle separately before the main draw loop)

---

### Step 4 — Compose the Robot Scene in Hw3.cpp

Replace the 6 farm `SceneObject`s:
- Change window title to `"Homework 3 - Robot Scene"`
- Load 5+ robot/sci-fi models
- **Position requirements** (spread across scene, not at origin):
  - At least 3 objects with non-zero rotation (varied yaw angles)
  - At least 2 with non-uniform scale (e.g., `vec3(1.5f, 2.0f, 1.5f)`)
  - At least 3 with a diffuse texture that loads successfully
- Move the point light to a center position (e.g., `(0, 3, 0)`) to simulate a ceiling light or beacon

---

### Step 5 — Write README.md

Required for +10 bonus. Location: `Homework_3/README.md`
- Build steps (cmake + make)
- Controls (WASD, mouse, scroll, ESC)
- Asset credits / model sources
- 30–60 second screen recording note (recorded separately)

---

## Critical Files

| File | Status | Changes Needed |
|---|---|---|
| `src/Hw3.cpp` | Farm-themed | Replace 6 SceneObjects + add ground plane |
| `src/model.cpp` | Missing error check + flag | Add CalcTangentSpace + error check |
| `src/shader.h` | Stray `#endif` bug | Remove it |
| `shaders/object.frag` | Lighting direction bug | Negate direction in CalcDirLight |
| `assets/` | Needs validation | Test .glb; fallback to OBJ if needed |
| `README.md` | Missing | Create for bonus |

---

## Verification

1. Clean build from empty `build/`: `cmake .. && make`
2. Scene opens — 5+ robot models visible, not all at origin
3. WASD + mouse navigation works
4. Specular highlights shift as camera orbits an object
5. Ground plane visible and textured (tiling)
6. At least 3 objects have visible textures (not flat gray)
