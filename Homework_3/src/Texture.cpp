/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-03-15
 * Description:  Homework 3: 3D World Scene Rendering with OpenGL and Assimp
 *
 *
 * @file Texture.cpp
 * @brief Texture class implementation for loading and managing textures in OpenGL. The Texture class is responsible for loading textures
 * from files, handling embedded textures in Assimp materials, and creating OpenGL texture objects. The implementation includes error
 * handling for texture loading and provides a fallback white texture if loading fails.
 *
 */

#include "Texture.h"
#include <array>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <algorithm>
#include <stb_image.h>
#include <string>

auto GetWhiteTexture() -> GLuint
{
    static GLuint whiteTexture = 0; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    if (whiteTexture == 0)
    {
        constexpr std::array<unsigned char, 4> white{255, 255, 255, 255};
        glGenTextures(1, &whiteTexture);
        glBindTexture(GL_TEXTURE_2D, whiteTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     white.data()); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR); // Use linear filtering for minification, smooth blocky textures will look better than nearest neighbor
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Use linear filtering for magnification
        std::cout << "Created white fallback texture: " << whiteTexture << "\n";
    }
    return whiteTexture;
}

auto LoadTextureFromFile(const std::string &path) -> GLuint
{
    // std::cout << "Attempting to load texture from file: " << path << "\n";

    int width = 0;
    int height = 0;
    int channels = 0;

    // Load the image using stb_image, forcing 4 channels (RGBA) for consistency
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 4);

    // If loading failed, fall back to the white texture
    if (data == nullptr)
    {
        std::cerr << "Failed to load texture: " << path << "\n";
        return GetWhiteTexture();
    }

    GLuint textureID = 0;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::cout << "Loaded texture from file: " << width << "x" << height << " (ID: " << textureID << ")"
              << "\n";
    stbi_image_free(data);

    return textureID;
}

// These are private helper functions for LoadTextureFromMaterial, so we can keep the main function cleaner and easier to read. They are
// defined in an unnamed namespace to limit their scope to this file only.
namespace
{
// @brief Load a diffuse texture from an Assimp material. This function checks for diffuse textures in the material, handles both embedded
// and external textures, and creates an OpenGL texture object. If no valid diffuse texture is found, it returns 0 to indicate failure.
// @param scene The Assimp scene containing the material and any embedded textures
// @param material The Assimp material to check for diffuse textures
// @param modelDirectory The directory of the model file, used to resolve relative paths for external textures
// @return The OpenGL texture ID of the loaded diffuse texture, or 0 if no valid texture was found
auto LoadDiffuseTexture(const aiScene *scene, const aiMaterial *material, const std::string &modelDirectory) -> GLuint
{
    unsigned int diffuse_count = material->GetTextureCount(aiTextureType_DIFFUSE);
    std::cout << "  Diffuse texture count: " << diffuse_count << "\n";

    if (diffuse_count == 0)
    {
        return 0; // No diffuse texture found
    }

    // Get the diffuse texture path
    aiString path;
    material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

    // Clean the path by removing comments (everything after #)
    // MTL files can have inline comments like: map_Kd Barrel_textures/barrel_BaseColor.png  # Diffuse texture map

    std::string clean_path(path.C_Str());
    size_t comment_pos = clean_path.find('#');
    if (comment_pos != std::string::npos)
    {
        clean_path = clean_path.substr(0, comment_pos);
    }

    // Trim trailing whitespace
    while (!clean_path.empty() && std::isspace(clean_path.back()))
    {
        clean_path.pop_back();
    }

    std::cout << "  Texture path: " << clean_path << "\n";

    // Construct full path relative to model directory
    // TODO(cwagenberg): replace with cross-platform path handling (e.g., std::filesystem) to handle different path separators and edge
    // cases
    std::string full_path = modelDirectory + "/" + clean_path;
    std::cout << "  Full path: " << full_path << "\n";

    return LoadTextureFromFile(full_path);
}

// @brief Load a diffuse color from an Assimp material and create a 1x1 OpenGL texture with that color. This function checks if the material
// has a diffuse color defined, and if so, creates a 1x1 texture filled with that color. If no diffuse color is found, it returns 0 to
// indicate failure.
// @param material The Assimp material to check for a diffuse color
// @return The OpenGL texture ID of the created 1x1 diffuse color texture, or 0 if no diffuse color was found
auto LoadDiffuseColor(const aiMaterial *material) -> GLuint
{
    aiColor3D diffuse_color(0.7F, 0.7F, 0.7F); // Default gray
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color) != AI_SUCCESS)
    {
        return 0; // No diffuse color found
    }

    std::cout << "  Using material diffuse color: RGB[" << diffuse_color.r << ", " << diffuse_color.g << ", " << diffuse_color.b << "]\n";

    // Create colored texture
    GLuint texture_id = 0;
    unsigned char color_data[4] = {
        static_cast<unsigned char>(diffuse_color.r * 255), static_cast<unsigned char>(diffuse_color.g * 255),
        static_cast<unsigned char>(diffuse_color.b * 255),
        255 // Alpha channel
    };

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, color_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return texture_id;
}
} // namespace

auto LoadTextureFromMaterial(const aiScene *scene, const aiMaterial *material, const std::string &modelDirectory) -> GLuint
{
    std::cout << "  LoadTextureFromMaterial: Checking material\n";

    // Try to load diffuse texture from file
    GLuint texture_id = LoadDiffuseTexture(scene, material, modelDirectory);
    if (texture_id != 0)
    {
        return texture_id;
    }

    // No texture file found, try to load diffuse color and create a 1x1 texture
    texture_id = LoadDiffuseColor(material);
    if (texture_id != 0)
    {
        return texture_id;
    }

    // No valid texture or color found, use white fallback
    std::cout << "  Using white fallback texture\n";
    return GetWhiteTexture();
}

auto CreateGrassTexture() -> GLuint
{
    const int texture_width = 256;
    const int texture_height = 256;

    // RGB format, 3 channels per pixel
    unsigned char grass_data[texture_width * texture_height * 3];

    // Create a simple grass texture with variations
    for (int y = 0; y < texture_height; ++y)
    {
        for (int x = 0; x < texture_width; ++x)
        {
            int index = ((y * texture_width) + x) * 3;

            // Base grass color (green)
            unsigned char red = 34;
            unsigned char green = 139;
            unsigned char blue = 34;

            // Add some variation
            // Create a checkerboard pattern of slightly different greens to simulate grass variation
            // e.g., every 32 pixels, we can slightly brighten the green channel to create a subtle pattern
            int variation = ((x + y) / 32) % 2;
            if (variation == 0)
            {
                green += 20; // Brighten the green channel for this pixel
            }

            grass_data[index] = red;
            grass_data[index + 1] = green;
            grass_data[index + 2] = blue;
        }
    }

    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, grass_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

auto GetShininessFromMaterial(const aiMaterial *material) -> float
{
    float shininess = 32.0F; // Default shininess value
    if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
    {
        // Clamp shininess to a reasonable range to prevent overly shiny or dull materials
        // that could wash out colors or make rendering appear incorrect
        constexpr float kMinShininess = 1.0F;
        constexpr float kMaxShininess = 200.0F;
        float clamped_shininess = std::max(kMinShininess, std::min(shininess, kMaxShininess));
        std::cout << "  Loaded shininess from material: " << shininess << " (clamped to " << clamped_shininess << ")\n";
        return clamped_shininess;
    }

    std::cout << "  Using default shininess: " << shininess << "\n";
    return shininess;
}
