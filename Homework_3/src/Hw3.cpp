#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main()
{
    glfwSetErrorCallback([](int, const char *err) { std::cerr << "GLFW Error: " << err << std::endl; });

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Test", nullptr, nullptr);
    if (!window)
        return -1;

    glfwMakeContextCurrent(window);
    glewInit();

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
