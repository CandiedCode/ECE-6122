#include "Texture.h"
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
    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 4);

    if (data == nullptr) // NOLINT(readability-implicit-bool-conversion)
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

auto LoadTextureFromMaterial(const aiScene *scene, const aiMaterial *material) -> GLuint
{
    std::cout << "  LoadTextureFromMaterial: Checking material"
              << "\n";
    std::cout << "  Number of embedded textures in scene: " << scene->mNumTextures << "\n";

    // Try to get diffuse texture
    unsigned int diffuseCount = material->GetTextureCount(aiTextureType_DIFFUSE);
    std::cout << "  Diffuse texture count: " << diffuseCount << "\n";

    if (diffuseCount > 0)
    {
        aiString path;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        std::cout << "  Texture path: " << path.C_Str() << "\n";

        // Check if it's an embedded texture
        const aiTexture *embeddedTexture = scene->GetEmbeddedTexture(path.C_Str());
        if (embeddedTexture != nullptr) // NOLINT(readability-implicit-bool-conversion)
        {
            std::cout << "  Found embedded texture: " << path.C_Str() << "\n";
            GLuint textureID = 0;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            // Handle compressed textures
            if (embeddedTexture->mHeight == 0)
            {
                // Compressed format - decompress with stb_image
                std::cout << "  Decompressing texture with stb_image"
                          << "\n";
                int width = 0;
                int height = 0;
                int channels = 0;
                unsigned char *data =
                    stbi_load_from_memory(reinterpret_cast<unsigned char *>(embeddedTexture->pcData), // NOLINT(clang-analyzer-unix.Malloc)
                                          static_cast<int>(embeddedTexture->mWidth), &width, &height, &channels, 4);

                if (data != nullptr) // NOLINT(readability-implicit-bool-conversion)
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    std::cout << "  Loaded decompressed texture: " << width << "x" << height << " (ID: " << textureID << ")"
                              << "\n";
                    stbi_image_free(data);
                    return textureID;
                }
                std::cout << "  Failed to decompress texture with stb_image"
                          << "\n";
            }
            else
            {
                // Uncompressed RGBA
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(embeddedTexture->mWidth),
                             static_cast<GLsizei>(embeddedTexture->mHeight), 0, GL_RGBA, GL_UNSIGNED_BYTE, embeddedTexture->pcData);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                std::cout << "  Loaded embedded texture: " << embeddedTexture->mWidth << "x" << embeddedTexture->mHeight
                          << " (ID: " << textureID << ")"
                          << "\n";
                return textureID;
            }
        }
        else
        {
            std::cout << "  No embedded texture found for: " << path.C_Str() << "\n";
        }
    }

    // No valid texture found, use white fallback
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
