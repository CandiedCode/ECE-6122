/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-03-15
 * Description:  Homework 3: 3D World Scene Rendering with OpenGL and Assimp
 *
 *
 * @file Texture.h
 * @brief Header file for texture loading utilities
 */

#ifndef HOMEWORK_3_SRC_TEXTURE_H_
#define HOMEWORK_3_SRC_TEXTURE_H_

#include <assimp/scene.h>
#include <glad/glad.h>
#include <string>

// Load a texture from file path
auto LoadTextureFromFile(const std::string &path) -> GLuint;

// Load texture from Assimp material (embedded or external)
auto LoadTextureFromMaterial(const aiScene *scene, const aiMaterial *material) -> GLuint;

#endif // HOMEWORK_3_SRC_TEXTURE_H_
