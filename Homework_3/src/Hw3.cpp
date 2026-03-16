#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
// #include <GL/glew.h>

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

// Global camera and input state
Camera *g_camera = nullptr;
float g_lastX = WINDOW_WIDTH / 2.0F;
float g_lastY = WINDOW_HEIGHT / 2.0F;
bool g_firstMouse = true;
float g_deltaTime = 0.0F;
float g_lastFrame = 0.0F;

// GLFW input callbacks
void MouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (g_firstMouse)
    {
        g_lastX = xpos;
        g_lastY = ypos;
        g_firstMouse = false;
        return;
    }

    float xoffset = xpos - g_lastX;
    float yoffset = g_lastY - ypos; // reversed: y increases downward in screen space

    g_lastX = xpos;
    g_lastY = ypos;

    if (g_camera != nullptr)
    {
        g_camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (g_camera != nullptr)
    {
        g_camera->ProcessMouseScroll(yoffset);
    }
}

// @brief Initialize GLFW, create window, and setup OpenGL context
// @details Code adapted from https://github.com/opengl-tutorials/ogl/blob/master/tutorial02_red_triangle/tutorial02.cpp
// @return Pointer to GLFWwindow on success, nullptr on failure
auto initializeWindow() -> GLFWwindow *
{
    if (!glfwInit())
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
    g_camera = new Camera(glm::vec3(0.0F, 5.0F, 10.0F)); // Start position: slightly elevated, looking at origin

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

    // Load model
    std::cout << "Loading model..."
              << "\n";
    // Model model("./assets/batamax/batamax.obj");
    Model model("./assets/toy_story_bullseye.glb");
    std::cout << "Model loaded"
              << "\n";

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

        // Setup matrices
        glm::mat4 model_mat = glm::mat4(1.0F);
        model_mat = glm::translate(model_mat, glm::vec3(0.0F, 0.0F, 0.0F));
        model_mat = glm::rotate(model_mat, glm::radians(90.0F), glm::vec3(0.0F, 1.0F, 0.0F)); // Rotate 90 degrees around Y

        glm::mat4 view = g_camera->GetViewMatrix();

        glm::mat4 projection =
            glm::perspective(glm::radians(45.0F), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f, 100.0F);

        // Set uniforms
        shader.SetMat4("model", model_mat);
        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);

        // Normal matrix = inverse(transpose(mat3(model)))
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model_mat)));
        shader.SetMat3("normalMatrix", normalMatrix);

        // Lighting uniforms
        glm::vec3 sunDirection = glm::normalize(glm::vec3(0.3f, 1.0F, 0.3f));
        shader.SetVec3("sun.direction", sunDirection);
        shader.SetVec3("sun.ambient", glm::vec3(0.3f, 0.3f, 0.3f));
        shader.SetVec3("sun.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
        shader.SetVec3("sun.specular", glm::vec3(0.5f, 0.5f, 0.5f));

        glm::vec3 lanternPos = glm::vec3(1.0F, 1.0F, 1.0F);
        shader.SetVec3("lantern.position", lanternPos);
        shader.SetVec3("lantern.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
        shader.SetVec3("lantern.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        shader.SetVec3("lantern.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        shader.SetFloat("lantern.constant", 1.0F);
        shader.SetFloat("lantern.linear", 0.09f);
        shader.SetFloat("lantern.quadratic", 0.032f);

        shader.SetVec3("viewPos", g_camera->GetPosition());
        shader.SetFloat("shininess", 32.0F);
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

        // Draw model
        // Clear error queue before drawing
        while (glGetError() != GL_NO_ERROR)
        {
        }

        model.Draw(shader);

        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cerr << "GL Error after model.Draw(): " << err << "\n";
            // Check if there are more errors
            while ((err = glGetError()) != GL_NO_ERROR)
            {
                std::cerr << "  Additional error: " << err << "\n";
            }
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    if (g_camera)
    {
        delete g_camera;
        g_camera = nullptr;
    }
    glfwTerminate();

    return 0;
}
