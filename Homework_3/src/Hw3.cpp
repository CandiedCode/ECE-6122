#include "Camera.h"
#include "Model.h"
#include "SceneObject.h"
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

// Global camera and input state
Camera *g_camera = nullptr;           // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
float g_lastX = WINDOW_WIDTH / 2.0F;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
float g_lastY = WINDOW_HEIGHT / 2.0F; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
bool g_firstMouse = true;             // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
float g_deltaTime = 0.0F;             // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
float g_lastFrame = 0.0F;             // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

// GLFW input callbacks
void MouseCallback(GLFWwindow *window, double xpos, double ypos) // NOLINT(readability-avoid-const-params-in-decls)
                                                                 // window parameter unused but required by GLFW callback signature
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
        g_camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) // NOLINT(readability-avoid-const-params-in-decls)
{
    if (g_camera != nullptr)
    {
        g_camera->ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

// @brief Initialize GLFW, create window, and setup OpenGL context
// @details Code adapted from https://github.com/opengl-tutorials/ogl/blob/master/tutorial02_red_triangle/tutorial02.cpp
// @return Pointer to GLFWwindow on success, nullptr on failure
auto initializeWindow() -> GLFWwindow *
{
    if (glfwInit() == 0)
    {
        std::cerr << "Failed to initialize GLFW\n";
        return nullptr;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "HW3 - Model Rendering", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to open GLFW window\n";
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    return window;
}

auto main() -> int
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
    glfwMakeContextCurrent(window);

    // Create and initialize camera
    g_camera = new Camera(glm::vec3(0.0F, 8.0F, 35.0F)); // NOLINT(cppcoreguidelines-owning-memory)
    // Start position: elevated, looking at origin

    // Register input callbacks
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    // Hide and capture mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLAD to load OpenGL function pointers
    // load all the OpenGL function addresses and check for any errors
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }

    // Clear the screen to a dark gray color and enable depth testing
    glClearColor(0.1f, 0.1f, 0.1f, 1.0F);
    // Enable depth testing for correct 3D rendering
    // https://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/
    glEnable(GL_DEPTH_TEST);

    // Load shader
    Shader shader("./shaders/object.vert", "./shaders/object.frag");
    std::cout << "Shader ID: " << shader.ID << "\n";

    // Load models
    std::cout << "Loading models..."
              << "\n";
    auto *model_bullseye = new Model("./assets/toy_story_bullseye.glb"); // NOLINT(cppcoreguidelines-owning-memory)
    auto *model_buzz = new Model("./assets/buzz_lightyear.glb");         // NOLINT(cppcoreguidelines-owning-memory)
    auto *model_woody = new Model("./assets/woody.glb");                 // NOLINT(cppcoreguidelines-owning-memory)
    auto *model_slinky = new Model("./assets/slinky_dog_rigged.glb");    // NOLINT(cppcoreguidelines-owning-memory)
    auto *model_hamm = new Model("./assets/hamm.glb");                   // NOLINT(cppcoreguidelines-owning-memory)
    std::cout << "Models loaded"
              << "\n";

    // Create scene objects with transforms
    std::vector<SceneObject> scene_objects;
    scene_objects.push_back(
        SceneObject{model_bullseye, glm::vec3(0.0F, 0.0F, 0.0F), 0.0F, glm::vec3(0.0F, 1.0F, 0.0F), glm::vec3(1.0F, 1.0F, 1.0F), 32.0F});
    scene_objects.push_back(
        SceneObject{model_buzz, glm::vec3(3.0F, 8.0F, 20.0F), 180.0F, glm::vec3(0.0F, 1.0F, 0.0F), glm::vec3(1.5F, 1.5F, 1.5F), 32.0F});
    scene_objects.push_back(
        SceneObject{model_woody, glm::vec3(-15.0F, 0.0F, 15.0F), 90.0F, glm::vec3(0.0F, 1.0F, 0.0F), glm::vec3(0.5F, 1.0F, 0.8F), 32.0F});
    scene_objects.push_back(
        SceneObject{model_slinky, glm::vec3(-8.0F, 0.0F, 10.0F), 0.0F, glm::vec3(0.0F, 1.0F, 0.0F), glm::vec3(1.0F, 1.0F, 1.0F), 32.0F});
    scene_objects.push_back(SceneObject{model_hamm, glm::vec3(10.0F, 0.0F, -10.0F), 180.0F, glm::vec3(0.0F, 1.0F, 0.0F),
                                        glm::vec3(0.02F, 0.02F, 0.02F), 32.0F});

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    std::cout << "Starting render loop..."
              << "\n";
    int frameCount = 0;
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
    {
        // Delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        g_deltaTime = currentFrame - g_lastFrame;
        g_lastFrame = currentFrame;

        // Handle keyboard input
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            g_camera->ProcessKeyboard(Camera::kFORWARD, g_deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            g_camera->ProcessKeyboard(Camera::kBACKWARD, g_deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            g_camera->ProcessKeyboard(Camera::kLEFT, g_deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            g_camera->ProcessKeyboard(Camera::kRIGHT, g_deltaTime);
        }

        if (frameCount == 0)
        {
            std::cout << "Frame 0: Starting to render"
                      << "\n";
        }
        frameCount++;
        if (frameCount > 60)
        {
            frameCount = 0;
        }

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cerr << "GL Error after clear: " << err << "\n";
        }

        // Use shader
        shader.Use();
        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cerr << "GL Error after shader.Use(): " << err << "\n";
        }

        // Setup view and projection matrices (same for all objects)
        glm::mat4 view = g_camera->GetViewMatrix();
        glm::mat4 projection =
            glm::perspective(glm::radians(45.0F), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1F, 100.0F);

        // Set view and projection uniforms
        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);

        // Lighting uniforms (same for all objects)
        glm::vec3 sunDirection = glm::normalize(glm::vec3(0.3F, 1.0F, 0.3F));
        shader.SetVec3("sun.direction", sunDirection);
        shader.SetVec3("sun.ambient", glm::vec3(0.3F, 0.3F, 0.3F));
        shader.SetVec3("sun.diffuse", glm::vec3(0.7F, 0.7F, 0.7F));
        shader.SetVec3("sun.specular", glm::vec3(0.5F, 0.5F, 0.5F));

        glm::vec3 lanternPos = glm::vec3(1.0F, 1.0F, 1.0F);
        shader.SetVec3("lantern.position", lanternPos);
        shader.SetVec3("lantern.ambient", glm::vec3(0.1F, 0.1F, 0.1F));
        shader.SetVec3("lantern.diffuse", glm::vec3(0.5F, 0.5F, 0.5F));
        shader.SetVec3("lantern.specular", glm::vec3(0.5F, 0.5F, 0.5F));
        shader.SetFloat("lantern.constant", 1.0F);
        shader.SetFloat("lantern.linear", 0.09F);
        shader.SetFloat("lantern.quadratic", 0.032F);

        shader.SetVec3("viewPos", g_camera->GetPosition());
        shader.SetInt("diffuseMap", 0);

        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cerr << "GL Error after setting uniforms: " << err << "\n";
        }
        else if (frameCount == 0)
        {
            std::cout << "No errors from uniforms"
                      << "\n";
        }

        // Draw all scene objects
        for (const auto &obj : scene_objects)
        {
            // Compute model matrix: translate -> rotate -> scale
            glm::mat4 model_mat = glm::mat4(1.0F);
            model_mat = glm::translate(model_mat, obj.position);
            model_mat = glm::rotate(model_mat, glm::radians(obj.rotation_degrees), obj.rotation_axis);
            model_mat = glm::scale(model_mat, obj.scale);

            // Set model-specific uniforms
            shader.SetMat4("model", model_mat);
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model_mat)));
            shader.SetMat3("normalMatrix", normalMatrix);
            shader.SetFloat("shininess", obj.shininess);

            // Clear error queue before drawing
            while (glGetError() != GL_NO_ERROR)
            {
            }

            obj.model->Draw(shader);
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    // Delete models from scene objects (track deleted pointers to avoid double-delete)
    std::vector<Model *> deleted_models;
    for (auto &obj : scene_objects)
    {
        if (obj.model != nullptr)
        {
            // Check if we've already deleted this model pointer
            bool already_deleted = false;
            for (const auto *deleted_ptr : deleted_models)
            {
                if (deleted_ptr == obj.model)
                {
                    already_deleted = true;
                    break;
                }
            }
            if (!already_deleted)
            {
                delete obj.model;
                deleted_models.push_back(obj.model);
                obj.model = nullptr;
            }
        }
    }

    if (g_camera != nullptr)
    {
        delete g_camera;
        g_camera = nullptr;
    }
    glfwTerminate();

    return 0;
}
