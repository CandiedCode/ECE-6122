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
    std::string vertex_code;
    std::ifstream v_shader_file(vertPath);
    if (v_shader_file.is_open())
    {
        std::stringstream v_shader_stream;
        v_shader_stream << v_shader_file.rdbuf();
        vertex_code = v_shader_stream.str();
        v_shader_file.close();
    }
    else
    {
        std::cerr << "Failed to open vertex shader: " << vertPath << "\n";
        exit(EXIT_FAILURE);
    }

    // Read fragment shader source
    std::string fragment_code;
    std::ifstream f_shader_file(fragPath);
    if (f_shader_file.is_open())
    {
        std::stringstream f_shader_stream;
        f_shader_stream << f_shader_file.rdbuf();
        fragment_code = f_shader_stream.str();
        f_shader_file.close();
    }
    else
    {
        std::cerr << "Failed to open fragment shader: " << fragPath << "\n";
        exit(EXIT_FAILURE);
    }

    // https://github.com/opengl-tutorials/ogl/blob/master/common/shader.cpp
    const char *v_shader_code = vertex_code.c_str();
    const char *f_shader_code = fragment_code.c_str();

    // Compile vertex shader
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_shader_code, nullptr);
    glCompileShader(vertex);

    // Check vertex compile errors
    GLint success = GL_FALSE;
    int info_log_length = 0;

    // Check Vertex Shader
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &info_log_length);

    if (info_log_length > 0)
    {
        std::vector<char> vertex_shader_error_message(info_log_length + 1);
        glGetShaderInfoLog(vertex, info_log_length, nullptr, &vertex_shader_error_message[0]);
        std::cerr << "Vertex shader compilation failed:\n" << &vertex_shader_error_message[0] << "\n";
    }

    // Compile fragment shader
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_shader_code, nullptr);
    glCompileShader(fragment);

    // Check fragment compile errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0)
    {
        std::vector<char> fragment_shader_error_message(info_log_length + 1);
        glGetShaderInfoLog(fragment, info_log_length, nullptr, &fragment_shader_error_message[0]);
        std::cerr << "Fragment shader compilation failed:\n" << &fragment_shader_error_message[0] << "\n";
    }

    // Link program
    id_ = glCreateProgram();
    glAttachShader(id_, vertex);
    glAttachShader(id_, fragment);
    glLinkProgram(id_);

    // Check the program
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0)
    {
        std::vector<char> program_error_message(info_log_length + 1);
        glGetProgramInfoLog(id_, info_log_length, nullptr, &program_error_message[0]);
        std::cerr << "Shader program linking failed:\n" << &program_error_message[0] << "\n";
    }

    // Delete shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    if (id_ != 0)
    {
        glDeleteProgram(id_);
    }
}

void Shader::Use() const
{
    glUseProgram(id_);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &val) const
{
    // Get the location of the uniform variable in the shader program
    GLint location = glGetUniformLocation(id_, name.c_str());
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
    GLint location = glGetUniformLocation(id_, name.c_str());
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
    GLint location = glGetUniformLocation(id_, name.c_str());
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
    GLint location = glGetUniformLocation(id_, name.c_str());
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
    GLint location = glGetUniformLocation(id_, name.c_str());
    if (location == -1)
    {
        std::cerr << "Uniform not found: " << name << "\n";
    }
    else
    {
        glUniform1i(location, val);
    }
}
