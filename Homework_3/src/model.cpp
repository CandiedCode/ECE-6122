#include "model.h"
#include "mesh.h"
#include "shader.h"
#include <algorithm>
#include <cfloat>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Model::Model(const std::string &path)
{
    Assimp::Importer importer;
    const aiScene *scene =
        importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Assimp Error loading '" << path << "': " << importer.GetErrorString() << std::endl;
        return;
    }

    std::cout << "Successfully loaded model: " << path << " (" << scene->mNumMeshes << " meshes)" << std::endl;
    directory = path.substr(0, path.find_last_of('/'));
    ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    for (unsigned i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }

    for (unsigned i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

GLuint Model::LoadTextureFromScene(const aiScene *scene, const aiMaterial *material)
{
    std::cout << "    LoadTextureFromScene: Checking material" << std::endl;
    std::cout << "    Number of embedded textures in scene: " << scene->mNumTextures << std::endl;

    // Try to get diffuse texture
    unsigned int diffuseCount = material->GetTextureCount(aiTextureType_DIFFUSE);
    std::cout << "    Diffuse texture count: " << diffuseCount << std::endl;

    if (diffuseCount > 0)
    {
        aiString path;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        std::cout << "    Texture path: " << path.C_Str() << std::endl;

        // Check if it's an embedded texture
        const aiTexture *embeddedTexture = scene->GetEmbeddedTexture(path.C_Str());
        if (embeddedTexture)
        {
            std::cout << "    Found embedded texture: " << path.C_Str() << std::endl;
            GLuint textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            // Handle compressed textures
            if (embeddedTexture->mHeight == 0)
            {
                // Compressed format - decompress with stb_image
                std::cout << "    Decompressing texture with stb_image" << std::endl;
                int width, height, channels;
                unsigned char *data = stbi_load_from_memory(reinterpret_cast<unsigned char *>(embeddedTexture->pcData),
                                                            embeddedTexture->mWidth, &width, &height, &channels, 4);

                if (data)
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    std::cout << "    Loaded decompressed texture: " << width << "x" << height << std::endl;
                    stbi_image_free(data);
                    return textureID;
                }
                else
                {
                    std::cout << "    Failed to decompress texture with stb_image" << std::endl;
                }
            }
            else
            {
                // Uncompressed RGBA
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, embeddedTexture->mWidth, embeddedTexture->mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                             embeddedTexture->pcData);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                std::cout << "    Loaded embedded texture: " << embeddedTexture->mWidth << "x" << embeddedTexture->mHeight << std::endl;
                return textureID;
            }
        }
        else
        {
            std::cout << "    No embedded texture found for: " << path.C_Str() << std::endl;
        }
    }

    // No valid texture found, use white fallback
    std::cout << "    Using white fallback texture" << std::endl;
    return Mesh::GetWhiteTexture();
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Log bounds
    float minX = FLT_MAX, maxX = -FLT_MAX;
    float minY = FLT_MAX, maxY = -FLT_MAX;
    float minZ = FLT_MAX, maxZ = -FLT_MAX;

    for (unsigned i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex v;
        v.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        v.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        if (mesh->mTextureCoords[0])
            v.TexCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        else
            v.TexCoords = {0.0f, 0.0f};
        vertices.push_back(v);

        minX = std::min(minX, v.Position.x);
        maxX = std::max(maxX, v.Position.x);
        minY = std::min(minY, v.Position.y);
        maxY = std::max(maxY, v.Position.y);
        minZ = std::min(minZ, v.Position.z);
        maxZ = std::max(maxZ, v.Position.z);
    }

    std::cout << "  Mesh bounds: X[" << minX << " to " << maxX << "] Y[" << minY << " to " << maxY << "] Z[" << minZ << " to " << maxZ
              << "]" << std::endl;

    for (unsigned i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Load texture from material
    Mesh result(vertices, indices);
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        result.textureID = LoadTextureFromScene(scene, material);
    }

    return result;
}

void Model::Draw(Shader &shader) const
{
    static int drawCount = 0;
    if (drawCount == 0)
    {
        std::cout << "Model::Draw() called with " << meshes.size() << " meshes" << std::endl;
    }
    drawCount++;
    if (drawCount > 60)
        drawCount = 0;

    for (const auto &mesh : meshes)
    {
        mesh.Draw();
    }
}
