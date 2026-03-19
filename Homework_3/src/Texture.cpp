#include "Texture.h"
#include <array>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        std::cout << "Created white fallback texture: " << whiteTexture << "\n";
    }
    return whiteTexture;
}

auto LoadTextureFromFile(const std::string &path) -> GLuint
{
    std::cout << "Attempting to load texture from file: " << path << "\n";
    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 4);

    // If loading failed, try alternative paths (in case of relative path issues)
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

auto LoadTextureFromMaterial(const aiScene *scene, const aiMaterial *material, const std::string &modelDirectory) -> GLuint
{
    std::cout << "  LoadTextureFromMaterial: Checking material"
              << "\n";

    // Try to get diffuse texture path
    unsigned int diffuseCount = material->GetTextureCount(aiTextureType_DIFFUSE);
    std::cout << "  Diffuse texture count: " << diffuseCount << "\n";

    if (diffuseCount > 0)
    {
        aiString path;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

        // Remove comments from path (everything after #)
        std::string cleanPath(path.C_Str());
        size_t commentPos = cleanPath.find('#');
        if (commentPos != std::string::npos)
        {
            cleanPath = cleanPath.substr(0, commentPos);
        }

        // Trim whitespace from the end
        while (!cleanPath.empty() && std::isspace(cleanPath.back()))
        {
            cleanPath.pop_back();
        }

        std::cout << "  Texture path: " << cleanPath << "\n";

        // Construct full path relative to model directory
        std::string fullPath = modelDirectory + "/" + cleanPath;
        std::cout << "  Full path: " << fullPath << "\n";

        // Try to load texture from file
        return LoadTextureFromFile(fullPath);
    }

    // No texture found, check for material color (Kd) and create a colored texture
    aiColor3D diffuseColor(0.7F, 0.7F, 0.7F); // Default gray
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
    {
        std::cout << "  Using material diffuse color: RGB(" << diffuseColor.r << ", " << diffuseColor.g << ", " << diffuseColor.b << ")\n";

        // Create a 1x1 colored texture (RGBA format for consistency)
        GLuint textureID = 0;
        unsigned char colorData[4] = {
            static_cast<unsigned char>(diffuseColor.r * 255), static_cast<unsigned char>(diffuseColor.g * 255),
            static_cast<unsigned char>(diffuseColor.b * 255),
            255 // Alpha channel (opaque)
        };

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorData);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        return textureID;
    }

    // No valid texture or color found, use white fallback
    std::cout << "  Using white fallback texture"
              << "\n";
    return GetWhiteTexture();
}

auto CreateGrassTexture() -> GLuint
{
    const int texWidth = 256;
    const int texHeight = 256;
    unsigned char grassData[texWidth * texHeight * 3];

    // Create a simple grass texture with variations
    for (int y = 0; y < texHeight; ++y)
    {
        for (int x = 0; x < texWidth; ++x)
        {
            int index = ((y * texWidth) + x) * 3;

            // Base grass color (green)
            unsigned char red = 34;
            unsigned char green = 139;
            unsigned char blue = 34;

            // Add some variation
            int variation = ((x + y) / 16) % 2;
            if (variation == 0)
            {
                green += 20;
            }

            grassData[index] = red;
            grassData[index + 1] = green;
            grassData[index + 2] = blue;
        }
    }

    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, grassData);
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
        std::cout << "  Loaded shininess from material: " << shininess << "\n";
    }
    else
    {
        std::cout << "  Using default shininess: " << shininess << "\n";
    }
    return shininess;
}
