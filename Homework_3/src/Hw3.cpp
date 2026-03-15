#include "camera.h"
#include "model.h"
#include "shader.h"
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
float g_lastX = WINDOW_WIDTH / 2.0f;
float g_lastY = WINDOW_HEIGHT / 2.0f;
bool g_firstMouse = true;
float g_deltaTime = 0.0f;
float g_lastFrame = 0.0f;

// // @brief Check if OpenGL context can be created and print version info
// // @return 0 on success, -1 on failure
// int checkOpenGLVersion()
// {
//     glfwSetErrorCallback([](int, const char *err) { std::cerr << "GLFW Error: " << err << std::endl; });

//     if (!glfwInit())
//         return -1;

//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//     GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Test", nullptr, nullptr);
//     if (!window)
//         return -1;

//     glfwMakeContextCurrent(window);
//     glewInit();

//     std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";
//     std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
//     std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
//     std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";

//     glfwDestroyWindow(window);
//     glfwTerminate();
//     return 0;
// }

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

    if (g_camera)
    {
        g_camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (g_camera)
    {
        g_camera->ProcessMouseScroll(yoffset);
    }
}

// @brief Initialize GLFW, create window, and setup OpenGL context
// @details Code adapted from https://github.com/opengl-tutorials/ogl/blob/master/tutorial02_red_triangle/tutorial02.cpp
// @return Pointer to GLFWwindow on success, nullptr on failure
GLFWwindow *initializeWindow()
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

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "HW3 - Model Rendering", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to open GLFW window\n";
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    return window;
}

int main(void)
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
    g_camera = new Camera(glm::vec3(0.0f, 5.0f, 10.0f)); // Start position: slightly elevated, looking at origin

    // Register input callbacks
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    // Hide and capture mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLAD to load OpenGL function pointers
    // load all the OpenGL function addresses and check for any errors
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }

    // Clear the screen to a dark gray color and enable depth testing
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // Enable depth testing for correct 3D rendering
    // https://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/
    glEnable(GL_DEPTH_TEST);

    // Load shader
    Shader shader("./shaders/object.vert", "./shaders/object.frag");
    std::cout << "Shader ID: " << shader.ID << std::endl;

    // Load model
    std::cout << "Loading model..." << std::endl;
    // Model model("./assets/batamax/batamax.obj");
    Model model("./assets/toy_story_bullseye.glb");
    std::cout << "Model loaded" << std::endl;

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    std::cout << "Starting render loop..." << std::endl;
    int frameCount = 0;
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
    {
        // Delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        g_deltaTime = currentFrame - g_lastFrame;
        g_lastFrame = currentFrame;

        // Handle keyboard input
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            g_camera->ProcessKeyboard(Camera::FORWARD, g_deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            g_camera->ProcessKeyboard(Camera::BACKWARD, g_deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            g_camera->ProcessKeyboard(Camera::LEFT, g_deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            g_camera->ProcessKeyboard(Camera::RIGHT, g_deltaTime);

        if (frameCount == 0)
        {
            std::cout << "Frame 0: Starting to render" << std::endl;
        }
        frameCount++;
        if (frameCount > 60)
            frameCount = 0;

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cerr << "GL Error after clear: " << err << std::endl;
        }

        // Use shader
        shader.Use();
        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cerr << "GL Error after shader.Use(): " << err << std::endl;
        }

        // Setup matrices
        glm::mat4 model_mat = glm::mat4(1.0f);
        model_mat = glm::translate(model_mat, glm::vec3(0.0f, 0.0f, 0.0f));
        model_mat = glm::rotate(model_mat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate 90 degrees around Y

        glm::mat4 view = g_camera->GetViewMatrix();

        glm::mat4 projection =
            glm::perspective(glm::radians(45.0f), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f, 100.0f);

        // Set uniforms
        shader.SetMat4("model", model_mat);
        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);

        // Normal matrix = inverse(transpose(mat3(model)))
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model_mat)));
        shader.SetMat3("normalMatrix", normalMatrix);

        // Lighting uniforms
        glm::vec3 sunDirection = glm::normalize(glm::vec3(0.3f, 1.0f, 0.3f));
        shader.SetVec3("sun.direction", sunDirection);
        shader.SetVec3("sun.ambient", glm::vec3(0.3f, 0.3f, 0.3f));
        shader.SetVec3("sun.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
        shader.SetVec3("sun.specular", glm::vec3(0.5f, 0.5f, 0.5f));

        glm::vec3 lanternPos = glm::vec3(1.0f, 1.0f, 1.0f);
        shader.SetVec3("lantern.position", lanternPos);
        shader.SetVec3("lantern.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
        shader.SetVec3("lantern.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        shader.SetVec3("lantern.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        shader.SetFloat("lantern.constant", 1.0f);
        shader.SetFloat("lantern.linear", 0.09f);
        shader.SetFloat("lantern.quadratic", 0.032f);

        shader.SetVec3("viewPos", g_camera->Position);
        shader.SetFloat("shininess", 32.0f);
        shader.SetInt("diffuseMap", 0);

        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cerr << "GL Error after setting uniforms: " << err << std::endl;
        }
        else if (frameCount == 0)
        {
            std::cout << "No errors from uniforms" << std::endl;
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
            std::cerr << "GL Error after model.Draw(): " << err << std::endl;
            // Check if there are more errors
            while ((err = glGetError()) != GL_NO_ERROR)
            {
                std::cerr << "  Additional error: " << err << std::endl;
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
