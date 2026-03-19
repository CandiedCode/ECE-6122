#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
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

    if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0 || scene->mRootNode == nullptr)
    {
        std::cerr << "Assimp Error loading '" << path << "': " << importer.GetErrorString() << "\n";
        return;
    }

    std::cout << "Successfully loaded model: " << path << " (" << scene->mNumMeshes << " meshes)"
              << "\n";
    ProcessNode(scene->mRootNode, scene);
}

Model::~Model()
{
    // Meshes in meshes_ vector will automatically be destroyed via their destructors,
    // which clean up their VAO, VBO, EBO, and texture resources
    meshes_.clear();
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

auto Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) -> Mesh
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Log bounds
    float minX = FLT_MAX;
    float maxX = -FLT_MAX;
    float minY = FLT_MAX;
    float maxY = -FLT_MAX;
    float minZ = FLT_MAX;
    float maxZ = -FLT_MAX;

    for (unsigned i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        vertex.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        if (mesh->mTextureCoords[0] != nullptr)
        {
            vertex.texture_coords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        }
        else
        {
            vertex.texture_coords = {0.0F, 0.0F};
        }
        vertices.push_back(vertex);

        minX = std::min(minX, vertex.position.x);
        maxX = std::max(maxX, vertex.position.x);
        minY = std::min(minY, vertex.position.y);
        maxY = std::max(maxY, vertex.position.y);
        minZ = std::min(minZ, vertex.position.z);
        maxZ = std::max(maxZ, vertex.position.z);
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

void Model::Draw() const
{
    for (const auto &mesh : meshes_)
    {
        mesh.Draw();
    }
}
