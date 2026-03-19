#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

constexpr int kWindowWidth = 1024;
constexpr int kWindowHeight = 768;

// Global camera and input state
Camera *g_camera = nullptr;           // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
float g_lastX = kWindowWidth / 2.0F;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
float g_lastY = kWindowHeight / 2.0F; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
bool g_firstMouse = true;             // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
float g_deltaTime = 0.0F;             // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
float g_lastFrame = 0.0F;             // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
bool g_equalKeyPressed = false;       // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
bool g_minusKeyPressed = false;       // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

// @brief Struct to represent an object in the scene with its model and transform
struct SceneObject
{
    Model *model;
    glm::vec3 position;
    float rotation_degrees;
    glm::vec3 rotation_axis;
    glm::vec3 scale;
};

// @brief GLFW mouse movement callback to update camera orientation based on mouse input
// @param window Pointer to GLFWwindow to query input state
// @param xpos The new x-coordinate of the mouse cursor
// @param ypos The new y-coordinate of the mouse cursor
// https://github.com/opengl-tutorials/ogl/blob/master/tutorial17_rotations/tutorial17.cpp#L97
void MouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (g_firstMouse)
    {
        g_lastX = static_cast<float>(xpos);
        g_lastY = static_cast<float>(ypos);
        g_firstMouse = false;
        return;
    }

    float xoffset = static_cast<float>(xpos) - g_lastX;
    float yoffset = g_lastY - static_cast<float>(ypos); // reversed: y increases downward in screen space

    g_lastX = static_cast<float>(xpos);
    g_lastY = static_cast<float>(ypos);

    if (g_camera != nullptr)
    {
        g_camera->ProcessCameraView(xoffset, yoffset);
    }
}

// @brief GLFW scroll callback to update camera movement speed based on scroll input
// @param window Pointer to GLFWwindow to query input state
// @param xoffset The horizontal scroll amount (not used in this implementation)
// @param yoffset The vertical scroll amount (positive for scroll up, negative for scroll down)
void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (g_camera != nullptr)
    {
        g_camera->ProcessMovementSpeed(static_cast<float>(yoffset));
    }
}

// @brief Clear all OpenGL errors by calling glGetError in a loop until it returns GL_NO_ERROR
void GLClearError()
{
    while (glGetError() != GL_NO_ERROR)
    {
        // Loop until all errors are cleared
    }
}

// @brief Resolve a file path by checking both the provided path and relative to the executable directory
// @details First checks if the file exists at the provided path. If not found, attempts to locate it relative to the executable directory.
// @param filePath The path to the file (can be relative or absolute)
// @param executablePath The path to the executable (argv[0])
// @return The resolved file path if found, std::nullopt otherwise
auto resolveFilePath(const std::string &filePath, const std::string &executablePath) -> std::optional<std::string>
{
    // Try the path as provided first
    if (std::filesystem::exists(filePath))
    {
        return filePath;
    }

    // Try relative to the executable directory
    std::string executable_dir = std::filesystem::path(executablePath).parent_path().string();
    std::string full_path = executable_dir + "/" + filePath;
    if (std::filesystem::exists(full_path))
    {
        return full_path;
    }

    // File not found in either location
    return std::nullopt;
}

// @brief Initialize GLFW, create window, and setup OpenGL context
// @details Attempts to detect screen size and open in fullscreen; falls back to windowed mode if detection fails
// @return Pointer to GLFWwindow on success, nullptr on failure
auto initializeWindow() -> GLFWwindow *
{
    if (glfwInit() == 0)
    {
        std::cerr << "Failed to initialize GLFW\n";
        return nullptr;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);                               // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // For MacOS compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We want the core profile

    // Try to detect screen size and use 80% of it
    GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
    int window_width = kWindowWidth;
    int window_height = kWindowHeight;

    if (primary_monitor != nullptr)
    {
        const GLFWvidmode *video_mode = glfwGetVideoMode(primary_monitor);
        if (video_mode != nullptr)
        {
            window_width = static_cast<int>(video_mode->width * 0.8);
            window_height = static_cast<int>(video_mode->height * 0.8);
            std::cout << "Detected screen resolution: " << video_mode->width << "x" << video_mode->height << ", using: " << window_width
                      << "x" << window_height << "\n";
        }
        else
        {
            std::cerr << "Failed to detect screen resolution, using default " << kWindowWidth << "x" << kWindowHeight << "\n";
        }
    }
    else
    {
        std::cerr << "Failed to detect primary monitor, using default " << kWindowWidth << "x" << kWindowHeight << "\n";
    }

    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "HW3 - Model Rendering", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to open GLFW window\n";
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window); // Initialize OpenGL context for the window

    return window;
}

// @brief Process keyboard input for camera movement
// @param window Pointer to GLFWwindow to query key states
// @param deltaTime Time elapsed since last frame to ensure consistent movement speed
void processKeyboard(GLFWwindow *window, float deltaTime)
{
    if (g_camera == nullptr)
    {
        return;
    }

    // W moves camera forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        std::cout << "W or Up arrow key pressed: moving camera forward\n";
        g_camera->ProcessDirection(Camera::kFORWARD, deltaTime);
    }
    // S moves camera backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        std::cout << "S or Down arrow key pressed: moving camera backward\n";
        g_camera->ProcessDirection(Camera::kBACKWARD, deltaTime);
    }
    // A moves camera left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        std::cout << "A or Left arrow key pressed: moving camera left\n";
        g_camera->ProcessDirection(Camera::kLEFT, deltaTime);
    }
    // D moves camera right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        std::cout << "D or Right arrow key pressed: moving camera right\n";
        g_camera->ProcessDirection(Camera::kRIGHT, deltaTime);
    }

    // Q quits the application
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        std::cout << "Q or Escape key pressed: exiting application\n";
        glfwSetWindowShouldClose(window, 1);
    }

    // + increases movement speed (only trigger on key press, not while held)
    bool equal_key_currently_pressed = glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS;
    if (equal_key_currently_pressed && !g_equalKeyPressed)
    {
        std::cout << "+ key pressed: increasing camera speed\n";
        g_camera->ProcessMovementSpeed(1.0F); // Simulate scroll up to increase speed
    }
    g_equalKeyPressed = equal_key_currently_pressed;

    // - decreases movement speed (only trigger on key press, not while held)
    bool minus_key_currently_pressed = glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS;
    if (minus_key_currently_pressed && !g_minusKeyPressed)
    {
        std::cout << "- key pressed: decreasing camera speed\n";
        g_camera->ProcessMovementSpeed(-1.0F); // Simulate scroll down to decrease speed
    }
    g_minusKeyPressed = minus_key_currently_pressed;
}

// @brief Load 3D models from disk using the Model class
// @return A vector of SceneObject structs containing the loaded models and their transforms
auto loadModels(const char *executablePath) -> std::vector<SceneObject>
{
    std::cout << "Loading models..."
              << "\n";

    // Load farmer model
    auto farmer_path = resolveFilePath("assets/models/farmer/farmer.obj", executablePath);
    if (!farmer_path.has_value())
    {
        std::cerr << "Error: Could not find farmer model at 'assets/models/farmer/farmer.obj' or relative to executable\n";
        exit(EXIT_FAILURE);
    }
    auto *model_farmer = new Model(farmer_path.value()); // NOLINT(cppcoreguidelines-owning-memory)

    // Load barrel model
    auto barrel_path = resolveFilePath("assets/models/barrel/Barrel_OBJ.obj", executablePath);
    if (!barrel_path.has_value())
    {
        std::cerr << "Error: Could not find barrel model at 'assets/models/barrel/Barrel_OBJ.obj' or relative to executable\n";
        exit(EXIT_FAILURE);
    }
    auto *model_barrel = new Model(barrel_path.value());   // NOLINT(cppcoreguidelines-owning-memory)
    auto *model_barrel_2 = new Model(barrel_path.value()); // NOLINT(cppcoreguidelines-owning-memory)

    // Load farmhouse model
    auto farmhouse_path = resolveFilePath("assets/models/farmhouse/Farm_house.obj", executablePath);
    if (!farmhouse_path.has_value())
    {
        std::cerr << "Error: Could not find farmhouse model at 'assets/models/farmhouse/Farm_house.obj' or relative to executable\n";
        exit(EXIT_FAILURE);
    }
    auto *model_farmhouse = new Model(farmhouse_path.value()); // NOLINT(cppcoreguidelines-owning-memory)

    // Load horse model
    auto horse_path = resolveFilePath("assets/models/Horse_Lores.obj", executablePath);
    if (!horse_path.has_value())
    {
        std::cerr << "Error: Could not find horse model at 'assets/models/Horse_Lores.obj' or relative to executable\n";
        exit(EXIT_FAILURE);
    }
    auto *model_horse = new Model(horse_path.value()); // NOLINT(cppcoreguidelines-owning-memory)

    // Load robot model
    auto robot_path = resolveFilePath("assets/models/robot/Robot.obj", executablePath);
    if (!robot_path.has_value())
    {
        std::cerr << "Error: Could not find robot model at 'assets/models/robot/Robot.obj' or relative to executable\n";
        exit(EXIT_FAILURE);
    }
    auto *model_robot = new Model(robot_path.value()); // NOLINT(cppcoreguidelines-owning-memory)

    std::cout << "Models loaded"
              << "\n";
    // Create scene objects with transforms
    std::vector<SceneObject> scene_objects;
    scene_objects.push_back(SceneObject{model_farmer, glm::vec3(0.0F, 3.0F, 0.0F),        // Positioned at origin
                                        0.0F,                                             // No rotation
                                        glm::vec3(0.0F, 1.0F, 0.0F),                      // Rotate around Y axis
                                        glm::vec3(5.0F, 5.0F, 5.0F)});                    // scale up to be more visible
    scene_objects.push_back(SceneObject{model_barrel, glm::vec3(-8.0F, 0.0F, 0.0F),       // Positioned to the left
                                        0.0F,                                             // No rotation
                                        glm::vec3(0.0F, 1.0F, 0.0F),                      // Rotate around Y axis
                                        glm::vec3(3.0F, 3.0F, 3.0F)});                    // scale up to be more visible
    scene_objects.push_back(SceneObject{model_barrel_2, glm::vec3(-10.25F, 0.0F, 0.0F),   // Positioned to the left
                                        0.0F,                                             // No rotation
                                        glm::vec3(0.0F, 1.0F, 0.0F),                      // Rotate around Y axis
                                        glm::vec3(3.0F, 3.0F, 3.0F)});                    // scale up to be more visible
    scene_objects.push_back(SceneObject{model_farmhouse, glm::vec3(20.0F, 10.0F, -20.0F), // Positioned far back and right
                                        90.0F,                                            // Rotate to face front
                                        glm::vec3(0.0F, 1.0F, 0.0F),                      // Rotate around Y axis to see front
                                        glm::vec3(0.25F, 0.25F, 0.25F)});                 // Much smaller scale to fit in scene
    scene_objects.push_back(SceneObject{model_horse, glm::vec3(8.0F, -5.0F, 40.0F),       // Positioned to the right and back
                                        180.0F,                                           // Rotate 180 degrees to face front
                                        glm::vec3(0.0F, 1.0F, 0.0F),                      // Rotate around Y axis
                                        glm::vec3(0.06F, 0.06F, 0.06F)});                 // Much smaller scale
    scene_objects.push_back(SceneObject{model_robot, glm::vec3(-15.0F, 0.0F, 25.0F),      // Positioned to the left and back
                                        0.0F,                                             // No rotation
                                        glm::vec3(0.0F, 1.0F, 0.0F),                      // Rotate around Y axis
                                        glm::vec3(1.0F, 1.0F, 1.0F)});                    // no scale

    return scene_objects;
}

// @brief Load ground plane model and texture
// @return A Mesh object representing the ground plane with the grass texture applied
auto loadGroundPlane() -> Mesh
{
    Mesh ground_mesh = CreateGroundPlane(100.0F, 10);
    ground_mesh.SetTextureID(CreateGrassTexture());
    std::cout << "Ground plane created"
              << "\n";
    return ground_mesh;
}

// @brief Main function initializes window, loads models and shaders, and enters render loop
// @param argc Argument count from command line
// @param argv Argument vector from command line (expects executable path in argv[0])
// @return Exit code (0 for success, non-zero for failure)
auto main(int argc, char **argv) -> int
{
    try
    {
        // Open a window and create its OpenGL context
        GLFWwindow *window = initializeWindow();
        if (window == nullptr)
        {
            std::cerr << "Failed to open GLFW window\n";
            getchar();
            glfwTerminate();
            return -1;
        }

        // Create and initialize camera
        g_camera = new Camera(glm::vec3(0.0F, 8.0F, 35.0F)); // NOLINT(cppcoreguidelines-owning-memory)
        // Start position: elevated, looking at origin

        // Register input callbacks
        // Set the mouse movement callback to update camera orientation based on mouse input
        glfwSetCursorPosCallback(window, MouseCallback);
        // Set the scroll callback to update camera zoom based on scroll input
        glfwSetScrollCallback(window, ScrollCallback);

        // Hide cursor but allow normal input (including trackpad scroll)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        // Initialize GLEW to load OpenGL function pointers
        // load all the OpenGL function addresses and check for any errors
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
        {
            std::cerr << "Failed to initialize GLEW\n";
            glfwTerminate();
            return -1;
        }

        // Clear the screen to a dark gray color and enable depth testing
        glClearColor(0.1F, 0.1F, 0.1F, 1.0F);
        // Enable depth testing for correct 3D rendering
        // https://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/
        glEnable(GL_DEPTH_TEST);

        // Load shader
        auto shader_path = resolveFilePath("assets/shaders/object.vert", argv[0]);
        if (!shader_path.has_value())
        {
            std::cerr << "Error: Could not find vertex shader at 'assets/shaders/object.vert' or relative to executable\n";
            exit(EXIT_FAILURE);
        }
        auto fragment_shader_path = resolveFilePath("assets/shaders/object.frag", argv[0]);
        if (!fragment_shader_path.has_value())
        {
            std::cerr << "Error: Could not find fragment shader at 'assets/shaders/object.frag' or relative to executable\n";
            exit(EXIT_FAILURE);
        }
        Shader shader(shader_path.value().c_str(), fragment_shader_path.value().c_str());
        std::cout << "Shader ID: " << shader.id_ << "\n";

        // Load 5 models
        auto scene_objects = loadModels(argv[0]);
        // Load grass texture and create ground plane mesh
        auto ground_mesh = loadGroundPlane();

        // Ensure we can capture the escape key being pressed below
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

        // Main render loop
        while (glfwWindowShouldClose(window) == 0)
        {
            // Delta time
            auto currentFrame = static_cast<float>(glfwGetTime());
            g_deltaTime = currentFrame - g_lastFrame;
            g_lastFrame = currentFrame;

            // Process keyboard input
            processKeyboard(window, g_deltaTime);

            // Clear screen
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Check for OpenGL errors after clearing the screen
            GLenum error = glGetError();
            if (error != GL_NO_ERROR)
            {
                std::cerr << "GL Error after clear: " << error << "\n";
            }

            // Use shader
            shader.Use();
            error = glGetError();
            if (error != GL_NO_ERROR)
            {
                std::cerr << "GL Error after shader.Use(): " << error << "\n";
            }

            // Setup view and projection matrices (same for all objects)
            glm::mat4 view = g_camera->GetViewMatrix();

            // Projection matrix : 45° Field of View, window width/height ratio, display range : 0.1 unit <-> 100 units
            // https://github.com/opengl-tutorials/ogl/blob/master/tutorial03_matrices/tutorial03.cpp#L70
            glm::mat4 projection =
                glm::perspective(glm::radians(45.0F), static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight), 0.1F, 100.0F);

            // Set view and projection uniforms
            // MVP is calculated in the shader (object.vert) to allow for normal matrix calculation and flexibility in shader design
            shader.SetMat4("view", view);
            shader.SetMat4("projection", projection);

            // Lighting uniforms (same for all objects)
            // Option 1: Warm Sunset (Golden Hour)
            glm::vec3 sunDirection = glm::normalize(glm::vec3(-0.5F, -0.8F, -0.2F));

            // https://learnopengl.com/Lighting/Light-casters
            // Set properties on uniform DirLight struct in shader for sun:
            shader.SetVec3("sun.direction", sunDirection);
            shader.SetVec3("sun.ambient", glm::vec3(0.4F, 0.35F, 0.25F)); // Warm ambient
            shader.SetVec3("sun.diffuse", glm::vec3(1.0F, 0.85F, 0.5F));  // Golden diffuse
            shader.SetVec3("sun.specular", glm::vec3(0.8F, 0.7F, 0.4F));

            // Lantern follows camera with offset (like a flashlight)
            glm::vec3 lanternPos =
                g_camera->GetPosition() + glm::vec3(2.0F, 1.0F, 2.0F); // Offset to the right, slightly above, and in front of the camera

            // Set properties on uniform PointLight struct in shader for lantern:
            shader.SetVec3("lantern.position", lanternPos);
            shader.SetVec3("lantern.ambient", glm::vec3(0.1F, 0.1F, 0.15F));
            shader.SetVec3("lantern.diffuse", glm::vec3(0.3F, 0.5F, 0.8F)); // Cool blue
            shader.SetVec3("lantern.specular", glm::vec3(0.5F, 0.5F, 1.0F));
            shader.SetFloat("lantern.constant", 1.0F);
            shader.SetFloat("lantern.linear", 0.09F);
            shader.SetFloat("lantern.quadratic", 0.032F);

            // Set viewer position uniform for specular lighting calculations
            shader.SetVec3("viewPos", g_camera->GetPosition());
            shader.SetInt("diffuseMap", 0);

            error = glGetError();
            if (error != GL_NO_ERROR)
            {
                std::cerr << "GL Error after setting uniforms: " << error << "\n";
            }

            // Draw all scene objects
            for (const auto &obj : scene_objects)
            {
                // Compute model matrix: translate -> rotate -> scale
                glm::mat4 model_mat = glm::mat4(1.0F);
                // Apply transformations in the correct order: scale
                model_mat = glm::translate(model_mat, obj.position);
                // Rotate around specified axis by specified degrees
                model_mat = glm::rotate(model_mat, glm::radians(obj.rotation_degrees), obj.rotation_axis);
                // Scale the model
                model_mat = glm::scale(model_mat, obj.scale);

                // Set model-specific uniforms
                shader.SetMat4("model", model_mat);
                glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model_mat)));
                shader.SetMat3("normalMatrix", normalMatrix);

                // Clear any existing OpenGL errors before drawing the model
                GLClearError();

                // Draw each mesh with its own shininess value
                for (const auto &mesh : obj.model->GetMeshes())
                {
                    shader.SetFloat("shininess", mesh.GetShininess());
                    mesh.Draw();
                }
            }

            // Draw ground plane
            glm::mat4 model_mat = glm::mat4(1.0F); // Ground plane is already positioned at the origin
            shader.SetMat4("model", model_mat);    // No transformations needed for the ground plane
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model_mat)));
            shader.SetMat3("normalMatrix", normalMatrix); // Normal matrix is identity since model matrix is identity
            shader.SetFloat("shininess", 15.0F);          // Ground plane shininess (less shiny than the toys)

            GLClearError();     // Clear any existing OpenGL errors before drawing the ground mesh
            ground_mesh.Draw(); // Draw the ground plane mesh

            // Swap buffers
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        // Cleanup OpenGL and application resources
        // Delete models (which triggers Mesh destructors to clean up VAO/VBO/EBO/textures)
        for (auto &obj : scene_objects)
        {
            if (obj.model != nullptr)
            {
                delete obj.model;
                obj.model = nullptr;
            }
        }

        if (g_camera != nullptr)
        {
            delete g_camera;
            g_camera = nullptr;
        }

        // Shader destructor will be called automatically here when shader goes out of scope
        // Ground mesh destructor will be called automatically when ground_mesh goes out of scope
        // Both will clean up their VAO/VBO/EBO and texture resources

        // Cleanup GLFW
        glfwDestroyWindow(window);
        glfwTerminate();

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Caught exception: " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception\n";
        return 1;
    }
}
