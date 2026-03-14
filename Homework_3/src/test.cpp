#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int checkOpenGLVersion()
{
    glfwSetErrorCallback([](int, const char *err) { std::cerr << "GLFW Error: " << err << std::endl; });

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Test", nullptr, nullptr);
    if (!window)
        return -1;

    glfwMakeContextCurrent(window);
    gladLoadGL();

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

int main()
{
    return checkOpenGLVersion();
}
