/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-03-15
 * Description:  Homework 3: 3D World Scene Rendering with OpenGL and Assimp
 *
 *
 * @file Shader.cpp
 * @brief Implementation file for the Shader class, which handles loading, compiling, and linking vertex and fragment shaders in OpenGL. The
 * Shader class provides methods to set uniform variables in the shader program, allowing for easy interaction with shader parameters during
 * rendering.  This code was adapted from the shader loading code in the OpenGL tutorial at
 * https://github.com/opengl-tutorials/ogl/blob/master/common/shader.cpp
 */

#include "Shader.h"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Shader::Shader(const char *vertPath, const char *fragPath)
{
    // Read vertex shader source
    std::string vertexCode;
    std::ifstream vShaderFile(vertPath);
    if (vShaderFile.is_open())
    {
        std::stringstream vShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        vertexCode = vShaderStream.str();
        vShaderFile.close();
    }
    else
    {
        std::cerr << "Failed to open vertex shader: " << vertPath << "\n";
        exit(EXIT_FAILURE);
    }

    // Read fragment shader source
    std::string fragmentCode;
    std::ifstream fShaderFile(fragPath);
    if (fShaderFile.is_open())
    {
        std::stringstream fShaderStream;
        fShaderStream << fShaderFile.rdbuf();
        fragmentCode = fShaderStream.str();
        fShaderFile.close();
    }
    else
    {
        std::cerr << "Failed to open fragment shader: " << fragPath << "\n";
        exit(EXIT_FAILURE);
    }

    // https://github.com/opengl-tutorials/ogl/blob/master/common/shader.cpp
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // Compile vertex shader
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);

    // Check vertex compile errors
    GLint success = GL_FALSE;
    int InfoLogLength = 0;

    // Check Vertex Shader
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if (InfoLogLength > 0)
    {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(vertex, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        std::cerr << "Vertex shader compilation failed:\n" << &VertexShaderErrorMessage[0] << "\n";
    }

    // Compile fragment shader
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);

    // Check fragment compile errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(fragment, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
        std::cerr << "Fragment shader compilation failed:\n" << &FragmentShaderErrorMessage[0] << "\n";
    }

    // Link program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    // Check the program
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        std::cerr << "Shader program linking failed:\n" << &ProgramErrorMessage[0] << "\n";
    }

    // Delete shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    if (ID != 0)
    {
        glDeleteProgram(ID);
    }
}

void Shader::Use() const
{
    glUseProgram(ID);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &val) const
{
    // Get the location of the uniform variable in the shader program
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1)
    {
        std::cerr << "Uniform not found: " << name << "\n";
    }
    else
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(val));
    }
}

void Shader::SetMat3(const std::string &name, const glm::mat3 &val) const
{
    // Get the location of the uniform variable in the shader program
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1)
    {
        std::cerr << "Uniform not found: " << name << "\n";
    }
    else
    {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(val));
    }
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &val) const
{
    // Get the location of the uniform variable in the shader program
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1)
    {
        std::cerr << "Uniform not found: " << name << "\n";
    }
    else
    {
        glUniform3f(location, val.x, val.y, val.z);
    }
}

void Shader::SetFloat(const std::string &name, float val) const
{
    // Get the location of the uniform variable in the shader program
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1)
    {
        std::cerr << "Uniform not found: " << name << "\n";
    }
    else
    {
        glUniform1f(location, val);
    }
}

void Shader::SetInt(const std::string &name, int val) const
{
    // Get the location of the uniform variable in the shader program
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1)
    {
        std::cerr << "Uniform not found: " << name << "\n";
    }
    else
    {
        glUniform1i(location, val);
    }
}
