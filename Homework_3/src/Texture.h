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

#include <GL/glew.h>
#include <assimp/scene.h>
#include <string>

// @brief Get a default white texture and return its OpenGL texture ID. This function creates a 1x1 white texture if it
// doesn't already exist, and returns the texture ID. The white texture can be used as a default when a mesh does not
// have an associated texture, ensuring that the mesh is still visible when rendered.
// @return The OpenGL texture ID of the white texture
auto GetWhiteTexture() -> GLuint;

// @brief Load a texture from an image file using stb_image and create an OpenGL texture object.
// @param path The file path to the image to be loaded as a texture
// @return The OpenGL texture ID of the loaded texture, or a fallback white texture if loading fails
auto LoadTextureFromFile(const std::string &path) -> GLuint;

// @brief Load a texture from an Assimp material. This function checks for diffuse textures in the material, handles both embedded and
// external textures, and creates an OpenGL texture object.
// @param scene The Assimp scene containing the material and any embedded textures
// @param material The Assimp material from which to load the texture
// @param modelDirectory The directory containing the model file (for resolving relative texture paths)
// @return The OpenGL texture ID of the loaded texture, or a fallback white texture if loading fails or if no valid texture is found in the
// material
auto LoadTextureFromMaterial(const aiScene *scene, const aiMaterial *material, const std::string &modelDirectory = ".") -> GLuint;

// @brief Create a simple procedural grass texture. This function generates a green texture with some variation to simulate grass and
// creates an OpenGL texture object.
// @return The OpenGL texture ID of the created grass texture
auto CreateGrassTexture() -> GLuint;

// @brief Extract shininess value from an Assimp material
// @param material The Assimp material from which to extract the shininess value
// @return The shininess value from the material, or a default value if not found
auto GetShininessFromMaterial(const aiMaterial *material) -> float;

#endif // HOMEWORK_3_SRC_TEXTURE_H_
