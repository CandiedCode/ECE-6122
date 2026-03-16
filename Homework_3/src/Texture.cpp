#include "Texture.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string>

// Static white fallback texture
static GLuint g_whiteTexture = 0;

GLuint GetWhiteTexture()
{
    if (g_whiteTexture == 0)
    {
        unsigned char white[] = {255, 255, 255, 255};
        glGenTextures(1, &g_whiteTexture);
        glBindTexture(GL_TEXTURE_2D, g_whiteTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        std::cout << "Created white fallback texture: " << g_whiteTexture << "\n";
    }
    return g_whiteTexture;
}

GLuint LoadTextureFromFile(const std::string &path)
{
    int width;
    int height;
    int channels;

    // TODO(cwagenberg): what is 4?
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 4);

    if (!data)
    {
        std::cerr << "Failed to load texture: " << path << "\n";
        // TODO(cwagenberg): raise error
        return GetWhiteTexture();
    }

    GLuint textureID;

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

GLuint LoadTextureFromMaterial(const aiScene *scene, const aiMaterial *material)
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
        if (embeddedTexture)
        {
            std::cout << "  Found embedded texture: " << path.C_Str() << "\n";
            GLuint textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            // Handle compressed textures
            if (embeddedTexture->mHeight == 0)
            {
                // Compressed format - decompress with stb_image
                std::cout << "  Decompressing texture with stb_image"
                          << "\n";
                int width, height, channels;
                unsigned char *data = stbi_load_from_memory(reinterpret_cast<unsigned char *>(embeddedTexture->pcData),
                                                            embeddedTexture->mWidth, &width, &height, &channels, 4);

                if (data)
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    std::cout << "  Loaded decompressed texture: " << width << "x" << height << " (ID: " << textureID << ")"
                              << "\n";
                    stbi_image_free(data);
                    return textureID;
                }
                else
                {
                    std::cout << "  Failed to decompress texture with stb_image"
                              << "\n";
                }
            }
            else
            {
                // Uncompressed RGBA
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, embeddedTexture->mWidth, embeddedTexture->mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                             embeddedTexture->pcData);
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
