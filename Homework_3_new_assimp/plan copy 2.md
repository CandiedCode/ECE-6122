# HW3 Pair-Programming Plan for Claude Code

## Goal
Finish **ECE 4122/6122 HW3: 3D World Scene Rendering with OpenGL and Assimp** in a way that is incremental, testable, and low-risk. The assignment requires OpenGL 3.3 Core Profile, VAO/VBO-based rendering, GLSL lighting, Assimp-based model loading, texturing, a free-fly camera, and a coherent outdoor scene with at least five distinct models. fileciteturn0file0L1-L18

## Current situation
- I want to work in **small verified steps**.
- I still need to download more `.obj` assets for the final scene.
- I currently have **1 OBJ** and **several GLB files** available for testing.
- The assignment explicitly says OBJ is required by default, but **GLB is allowed only if we verify Assimp loads it correctly**. It also warns to prefer OBJ unless verified. fileciteturn0file0L6-L18 fileciteturn0file0L89-L96
- I do **not** want to jump straight into the full scene. I want a working vertical slice first.

## Non-negotiable assignment requirements
Claude should keep these in mind at all times:

1. **OpenGL 3.3 Core Profile only**. No legacy fixed-function calls like `glBegin/glEnd`, `glLight*`, or `glMatrixMode`. fileciteturn0file0L14-L18
2. Use **GLAD**, **GLFW 3.4**, and **Assimp 5.x**. fileciteturn0file0L14-L18
3. Final scene needs **at least five distinct OBJ mesh files** at unique world positions, with rotations and non-uniform scaling requirements, plus a textured ground plane. fileciteturn0file0L22-L31
4. Implement **Mesh** and **Model** classes using Assimp, including:
   - positions, normals, texcoords
   - multiple meshes per model
   - diffuse textures from material data
   - recursive traversal of the Assimp node graph fileciteturn0file0L32-L39
5. Implement GLSL lighting with:
   - one directional light
   - one point light
   - correct normal matrix use
   - visible specular highlights that change with camera movement fileciteturn0file0L40-L45
6. Implement FPS-style camera:
   - WASD
   - mouse yaw/pitch
   - scroll speed adjustment
   - ESC exits
   - `glm::lookAt` view matrix fileciteturn0file0L46-L51
7. At least **three objects textured**, and **ground plane tiled**. fileciteturn0file0L53-L55
8. Must compile cleanly with the provided/project `CMakeLists.txt`; otherwise the grader may cap credit at 50%. fileciteturn0file0L106-L112

## Strategy: build a vertical slice first
We should not start by downloading all final assets or building the entire farm scene.

Instead, we should build this sequence:

1. **Window + shader + single hardcoded triangle/mesh sanity check**
2. **Single Assimp-loaded model renders successfully**
3. **Camera moves around that one model**
4. **Lighting works on that one model**
5. **Texture sampling works on that one model**
6. **Ground plane works**
7. **Scene object transform system works for multiple objects**
8. **Add remaining assets and tune layout**
9. **README + cleanup + clean rebuild verification**

This keeps each step debuggable and gives us a known-good checkpoint before moving on.

## Definition of success for the first verified step
The very first milestone should be:

> A clean build that opens a GLFW window and renders exactly **one Assimp-loaded model** with a basic shader and a fixed camera, even if textures, multiple lights, and the final scene are not done yet.

Why this is the right first step:
- It proves the hardest integration boundary early: **Assimp -> Mesh data -> VAO/VBO -> shader -> render loop**.
- It avoids wasting time building scene logic before model loading works.
- It lets us test whether my existing **OBJ** and **GLB** test files actually load with Assimp.

## Milestone plan

### Milestone 0 — Build and dependency sanity
**Goal:** confirm the project builds and links with OpenGL, GLFW, GLAD, GLM, and Assimp.

#### Tasks
- Inspect existing project structure and `CMakeLists.txt`.
- Make the smallest set of changes needed so the app compiles.
- Confirm shader files are found at runtime.
- Add clear error logging for shader compile failures and Assimp import failures.

#### Acceptance criteria
- Fresh build directory succeeds with CMake + build tool.
- Running the executable opens a window without crashing.
- No legacy OpenGL calls are used.

#### Notes for Claude
Prefer minimal, boring fixes over refactors. Do not redesign the whole project at this step.

---

### Milestone 1 — Single model vertical slice
**Goal:** load and draw one model through Assimp.

#### Tasks
- Implement `Vertex`, `Texture`, `Mesh`, and `Model` skeletons if missing.
- In `Model`, call `Assimp::Importer::ReadFile` with the assignment-recommended flags:
  - `aiProcess_Triangulate`
  - `aiProcess_FlipUVs`
  - `aiProcess_GenSmoothNormals`
  - `aiProcess_CalcTangentSpace` fileciteturn0file0L73-L79
- Implement recursive node traversal.
- Extract per-vertex:
  - position
  - normal
  - texture coordinates
- Extract indices from faces.
- Create VAO/VBO/EBO for the mesh.
- Render the model with a very simple shader.
- Start with the **known OBJ** first.
- After that works, optionally test one GLB file and log whether Assimp loaded it successfully.

#### Acceptance criteria
- The app renders one imported model on screen.
- The code handles at least one model with one or more meshes.
- Import failures print the Assimp error string.

#### Important rule
A GLB that loads successfully is useful for early testing, but the final submission should still prioritize **five OBJ assets** unless we are fully confident the grader and asset packaging will be fine.

---

### Milestone 2 — Camera before fancy scene work
**Goal:** make it easy to inspect the imported model.

#### Tasks
- Implement `Camera` class with:
  - position
  - front
  - up
  - yaw/pitch
  - movement speed
- Add `glm::lookAt` view matrix.
- Add WASD movement constrained to the XZ plane.
- Add mouse look.
- Add scroll wheel speed adjustment.
- Add ESC exit. fileciteturn0file0L46-L51

#### Acceptance criteria
- I can move around the model smoothly.
- Mouse look is intuitive and pitch is clamped.
- Scroll changes movement speed.

#### Why this comes early
Camera controls make all future debugging dramatically easier.

---

### Milestone 3 — Lighting on one object
**Goal:** implement the required lighting model before multiplying scene complexity.

#### Tasks
- Add vertex shader outputs for world-space fragment position, normal, and texcoords.
- Compute `normalMatrix = inverse(transpose(mat3(model)))` and pass it as a uniform. fileciteturn0file0L63-L70
- Add fragment shader with:
  - directional light (`sun`)
  - point light (`lantern`)
  - ambient + diffuse + specular
- Pass `viewPos` for specular highlights. fileciteturn0file0L40-L45

#### Acceptance criteria
- Surface shading changes as camera moves.
- Specular highlights are visible.
- Normals are transformed correctly.

---

### Milestone 4 — Texture loading and fallback behavior
**Goal:** support textured rendering without blocking progress on messy assets.

#### Tasks
- Add texture loading via `stb_image`.
- Load diffuse textures from Assimp material data when present. fileciteturn0file0L32-L39
- If texture loading fails, use a safe fallback behavior so the object still renders.
- Verify at least one textured model works.

#### Acceptance criteria
- One textured model renders correctly.
- Missing textures do not crash the program.

#### Asset note
The assignment says PNG/JPG are safest; TGA/DDS may need extra handling. fileciteturn0file0L89-L96

---

### Milestone 5 — Ground plane
**Goal:** satisfy the ground requirement early and create visual context.

#### Tasks
- Create a flat quad mesh in code.
- Subdivide it if useful, but keep it simple unless subdivision is needed.
- Apply a repeating grass or dirt texture.
- Tile the UVs so the texture repeats clearly. fileciteturn0file0L22-L31

#### Acceptance criteria
- Ground plane is visible.
- Ground texture repeats rather than stretching once.

---

### Milestone 6 — Scene object system
**Goal:** support required transforms cleanly.

#### Tasks
- Add a `SceneObject` struct similar to the assignment example with:
  - model pointer/reference
  - position
  - rotation degrees
  - rotation axis
  - scale
  - shininess fileciteturn0file0L80-L88
- In the render loop, compute model matrix and normal matrix per object.
- Verify translation, rotation, and scale each work independently.

#### Acceptance criteria
- Multiple objects can be placed at distinct world positions.
- At least three objects can have non-zero rotations.
- At least two can be non-uniformly scaled.

---

### Milestone 7 — Final asset integration
**Goal:** replace placeholders with final assignment-compliant assets.

#### Tasks
- Download at least five distinct OBJ assets with `.mtl` and textures where possible.
- Keep directory layout clean under `assets/`.
- Test each model individually before adding it to the full scene.
- Normalize scale either in `SceneObject.scale` or via Assimp/global-scale handling. The handout explicitly warns model scales may vary wildly. fileciteturn0file0L97-L100
- Build a coherent outdoor rural scene.

#### Acceptance criteria
- At least five distinct objects in scene.
- Layout is logical and not clustered at origin.
- At least three textured objects.
- Scene feels coherent enough for the rubric.

---

### Milestone 8 — Submission hardening
**Goal:** protect against silly grading losses.

#### Tasks
- Clean up warnings/errors.
- Verify a completely fresh build from a new `build/` directory.
- Write `README.md`:
  - build steps
  - controls
  - asset list / attribution if needed
  - note on tested platform
- Confirm ZIP should include sources, `CMakeLists.txt`, `shaders/`, `assets/`, and `README.md`, but not binaries or `build/`. fileciteturn0file0L106-L112
- Prepare short walkthrough video.

#### Acceptance criteria
- Clean rebuild works from scratch.
- README is enough for a TA to run the project.
- Submission folder contents match the handout.

## Implementation order inside the codebase
Claude should follow roughly this order unless the current repo strongly suggests a better one:

1. `shader.h/.cpp`
2. `mesh.h/.cpp`
3. `model.h/.cpp`
4. `camera.h/.cpp`
5. `texture.h/.cpp` or texture helpers
6. `main.cpp` scene/render integration
7. `shaders/object.vert` and `shaders/object.frag`
8. `README.md`

## Coding style guidance for Claude
- Keep changes incremental and compile often.
- Prefer small commits/patches over giant rewrites.
- Do not paste in a full LearnOpenGL implementation wholesale.
- Write code I can explain in class or to a TA.
- Keep headers and source files organized, but do not over-engineer abstractions.
- Add practical logging around file paths, shader errors, and Assimp import errors.
- When unsure, choose the simpler implementation that satisfies the rubric.

## Asset handling guidance
- For now, use my available **1 OBJ** as the first real loader target.
- Use my **GLB files only as test probes** to confirm whether Assimp can load them in my environment.
- Do not build the final deliverable around GLB unless we decide it is clearly safe.
- For final submission, prioritize:
  - models with `.obj` + `.mtl`
  - texture files in `.png` or `.jpg`
  - easy-to-package folder structures

## Testing checklist after each milestone
After every milestone, Claude should help me run a short checklist:

- Does it compile from the command line?
- Does it run without crashing?
- Are file paths correct from the executable working directory?
- Is there at least one visible thing on screen?
- Is the next bug isolated enough that we know where to look?

## Rubric mapping checklist
Use this before calling the project done:

### Scene composition
- [ ] 5+ distinct objects
- [ ] unique positions
- [ ] 3+ rotated objects
- [ ] 2+ non-uniformly scaled objects
- [ ] textured ground plane
- [ ] visually coherent layout

### Assimp mesh loading
- [ ] positions loaded
- [ ] normals loaded/generated
- [ ] texcoords loaded when present
- [ ] indices loaded from faces
- [ ] multi-mesh models supported
- [ ] diffuse textures loaded from material data
- [ ] recursive aiNode traversal implemented

### Lighting
- [ ] directional light
- [ ] point light with attenuation
- [ ] normal matrix used correctly
- [ ] visible specular highlights tied to camera movement

### Camera
- [ ] WASD on XZ plane
- [ ] mouse yaw/pitch
- [ ] scroll changes speed
- [ ] ESC exits
- [ ] `glm::lookAt` view matrix

### Texturing
- [ ] 3+ textured objects
- [ ] ground texture tiled

### Submission safety
- [ ] clean CMake build from fresh directory
- [ ] README present
- [ ] assets included
- [ ] no build artifacts in ZIP
- [ ] short video prepared

## Prompt to give Claude Code at the start
Use this as the opening instruction:

```text
We are finishing an OpenGL + Assimp homework. I want to work in very small, verified steps and avoid giant rewrites. Please first inspect the current project and help me reach one minimal vertical slice: a clean build that opens a GLFW window and renders one Assimp-loaded model with a simple shader. Start with my known OBJ asset first. Keep changes minimal, explain the plan before each patch, and pause after each verified step.
```

## Prompt for the next phase after single-model success
```text
Great. Now help me add the next smallest useful feature: FPS camera controls with WASD, mouse yaw/pitch, scroll-based movement speed, and ESC to quit. Keep the existing model rendering working the whole time.
```

## Prompt for asset verification
```text
I have some GLB files available, but the assignment prefers OBJ unless Assimp loading is verified. Please add a tiny diagnostic path or logging so we can confirm which files load successfully with Assimp, without refactoring the project.
```

## Prompt for final rubric pass
```text
Please compare the current project against the homework rubric and give me a gap list grouped by: scene composition, Assimp loading, lighting, camera, texturing, build/submission readiness. For each gap, suggest the smallest change that closes it.
```

## What not to do
- Do not jump straight to the full farm scene before one model renders.
- Do not do a large architectural rewrite unless a blocker truly requires it.
- Do not add optional features like skybox, fog, or shadows until the required rubric items are done.
- Do not rely on GLB for the final scene without explicitly deciding that risk is acceptable.
- Do not leave build reproducibility until the end.

## Final note
The handout provides a recommended structure, shader skeletons, Assimp flags, a sample `SceneObject`, and asset guidance. We should use that as a guardrail and implement only what we can understand and defend. fileciteturn0file0L56-L88
