#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char *vertPath, const char *fragPath)
{
    // Read vertex shader source
    std::string vertexCode;
    std::ifstream vShaderFile(vertPath);
    if (vShaderFile.is_open()) {
        std::stringstream vShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        vertexCode = vShaderStream.str();
        vShaderFile.close();
    } else {
        std::cerr << "Failed to open vertex shader: " << vertPath << std::endl;
    }

    // Read fragment shader source
    std::string fragmentCode;
    std::ifstream fShaderFile(fragPath);
    if (fShaderFile.is_open()) {
        std::stringstream fShaderStream;
        fShaderStream << fShaderFile.rdbuf();
        fragmentCode = fShaderStream.str();
        fShaderFile.close();
    } else {
        std::cerr << "Failed to open fragment shader: " << fragPath << std::endl;
    }

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // Compile vertex shader
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    // Check vertex compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    // Compile fragment shader
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    // Check fragment compile errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // Link program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    // Check link errors
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    }

    // Delete shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::Use() const
{
    glUseProgram(ID);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &val) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::SetMat3(const std::string &name, const glm::mat3 &val) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &val) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform3f(loc, val.x, val.y, val.z);
}

void Shader::SetFloat(const std::string &name, float val) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform1f(loc, val);
}

void Shader::SetInt(const std::string &name, int val) const
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform1i(loc, val);
}
