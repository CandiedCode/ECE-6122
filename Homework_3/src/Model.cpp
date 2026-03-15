#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "texture.h"
#include <algorithm>
#include <cfloat>
#include <iostream>
#include <string>
#include <vector>

Model::Model(const std::string &path)
{
    // Create an instance of the Open Asset Importer
    Assimp::Importer importer;

    const aiScene *scene =
        importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Assimp Error loading '" << path << "': " << importer.GetErrorString() << "\n";
        return;
    }

    std::cout << "Successfully loaded model: " << path << " (" << scene->mNumMeshes << " meshes)"
              << "\n";
    directory_ = path.substr(0, path.find_last_of('/'));
    ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    for (unsigned i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes_.push_back(ProcessMesh(mesh, scene));
    }

    for (unsigned i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene);
    }
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
        v.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        v.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        if (mesh->mTextureCoords[0])
            v.tex_coords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        else
            v.tex_coords = {0.0F, 0.0F};
        vertices.push_back(v);

        minX = std::min(minX, v.position.x);
        maxX = std::max(maxX, v.position.x);
        minY = std::min(minY, v.position.y);
        maxY = std::max(maxY, v.position.y);
        minZ = std::min(minZ, v.position.z);
        maxZ = std::max(maxZ, v.position.z);
    }

    std::cout << "  Mesh bounds: X[" << minX << " to " << maxX << "] Y[" << minY << " to " << maxY << "] Z[" << minZ << " to " << maxZ
              << "]"
              << "\n";

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
        result.SetTextureID(LoadTextureFromMaterial(scene, material));
    }

    return result;
}

void Model::Draw(Shader &shader) const
{
    static int draw_count = 0;
    if (draw_count == 0)
    {
        std::cout << "Model::Draw() called with " << meshes_.size() << " meshes"
                  << "\n";
    }
    draw_count++;
    if (draw_count > 60)
        draw_count = 0;

    for (const auto &mesh : meshes_)
    {
        mesh.Draw();
    }
}
