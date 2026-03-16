#pragma once

#include <assimp/scene.h>
#include <glad/glad.h>
#include <string>

// Load a texture from file path
GLuint LoadTextureFromFile(const std::string &path);

// Load texture from Assimp material (embedded or external)
GLuint LoadTextureFromMaterial(const aiScene *scene, const aiMaterial *material);

// Get or create white fallback texture
GLuint GetWhiteTexture();
