/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-03-15
 * Description:  Homework 3: 3D World Scene Rendering with OpenGL and Assimp
 *
 *
 * @file Shader.h
 * @brief Header file for loading and managing shaders in OpenGL. The Shader class encapsulates the functionality to load, compile, and link
 * vertex and fragment shaders, and provides methods to set uniform variables in the shaders.
 */

#ifndef HOMEWORK_3_SRC_SHADER_H_
#define HOMEWORK_3_SRC_SHADER_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Shader
{
  public:
    // The OpenGL ID of the shader program created by linking the vertex and fragment shaders together.
    // This ID is used to reference the shader program
    GLuint ID;

    // @brief Constructor that loads, compiles, and links vertex and fragment shaders from the specified file paths.
    // @param vertPath The file path to the vertex shader source code
    // @param fragPath The file path to the fragment shader source code
    Shader(const char *vertPath, const char *fragPath);

    // @brief Destructor that cleans up the shader program by calling glDeleteProgram.
    ~Shader();

    // @brief Activates the shader program for use in rendering. This method should be called before setting any uniform variables or
    // issuing draw calls that use this shader program.
    void Use() const;

    // @brief Utility functions to set uniform variables in the shader program. These functions take the name of the uniform variable and
    // the value to set it to, and update the shader program's uniform state accordingly.
    void SetMat4(const std::string &name, const glm::mat4 &val) const;

    // @brief Sets a mat3 uniform variable in the shader program.
    // @param name The name of the uniform variable in the shader
    // @param val The glm::mat3 value to set the uniform variable to
    void SetMat3(const std::string &name, const glm::mat3 &val) const;

    // @brief Sets a vec3 uniform variable in the shader program.
    // @param name The name of the uniform variable in the shader
    // @param val The glm::vec3 value to set the uniform variable to
    void SetVec3(const std::string &name, const glm::vec3 &val) const;

    // @brief Sets a float uniform variable in the shader program.
    // @param name The name of the uniform variable in the shader
    // @param val The float value to set the uniform variable to
    void SetFloat(const std::string &name, float val) const;

    // @brief Sets an int uniform variable in the shader program.
    // @param name The name of the uniform variable in the shader
    // @param val The int value to set the uniform variable to
    void SetInt(const std::string &name, int val) const;
};

#endif // HOMEWORK_3_SRC_SHADER_H_
