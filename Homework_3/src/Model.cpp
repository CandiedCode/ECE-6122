#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
#include <algorithm>
#include <cfloat>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

Model::~Model()
{
    // Meshes in meshes_ vector will automatically be destroyed via their destructors,
    // which clean up their VAO, VBO, EBO, and texture resources
    meshes_.clear();
}

Model::Model(const std::string &path)
{
    // Extract the directory from the model path
    // This is used to resolve relative texture paths in the model file
    modelDirectory_ = std::filesystem::path(path).parent_path().string();
    if (modelDirectory_.empty())
    {
        modelDirectory_ = ".";
    }

    // Create an instance of the Open Asset Importer
    Assimp::Importer importer;

    // https://the-asset-importer-lib-documentation.readthedocs.io/en/latest/usage/postprocessing.html
    unsigned int post_processing_flags =
        aiProcess_Triangulate         // Triangulate meshes to ensure all faces are triangles for rendering
        | aiProcess_SortByPType       // Sort meshes by primitive type to separate points, lines, and triangles
        | aiProcess_FlipUVs           // Used by VAO / VBOs to match OpenGL's coordinate system for textures,
        | aiProcess_GenSmoothNormals  // Generate smooth normals if not provided in the model file, which is important for proper lighting
                                      // calculations in the shader
        | aiProcess_CalcTangentSpace; // Calculate tangents and bi-tangents for normal mapping if the model contains texture coordinates

    const aiScene *scene = importer.ReadFile(path, post_processing_flags);

    if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0 || scene->mRootNode == nullptr)
    {
        std::cerr << "Assimp Error loading '" << path << "': " << importer.GetErrorString() << "\n";
        return;
    }
    // if ((scene->mFlags & AI_SCENE_FLAGS_VALIDATED) == 1)
    // {
    //     std::cout << "Assimp loaded successfully\n";
    // }

    std::cout << "Successfully loaded model: " << path << " with " << scene->mNumMeshes << " meshes\n";
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

auto Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) -> Mesh
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Log bounds
    float min_x = FLT_MAX;
    float max_x = -FLT_MAX;

    float min_y = FLT_MAX;
    float max_y = -FLT_MAX;

    float min_z = FLT_MAX;
    float max_z = -FLT_MAX;

    for (unsigned i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        vertex.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

        if (mesh->mTextureCoords[0] != nullptr)
        {
            vertex.texture_coords = {mesh->mTextureCoords[0][i].x,  // U
                                     mesh->mTextureCoords[0][i].y}; // V
                                                                    // z component of mTextureCoords is not used for 2D textures
        }
        else
        {
            // If the mesh does not have texture coordinates, we can set them to a default value (e.g., (0, 0)).
            vertex.texture_coords = {0.0F, 0.0F};
        }
        vertices.push_back(vertex);

        min_x = std::min(min_x, vertex.position.x);
        max_x = std::max(max_x, vertex.position.x);
        min_y = std::min(min_y, vertex.position.y);
        max_y = std::max(max_y, vertex.position.y);
        min_z = std::min(min_z, vertex.position.z);
        max_z = std::max(max_z, vertex.position.z);
    }

    std::cout << "  Mesh bounds: X[" << min_x << " - " << max_x << "] Y[" << min_y << " - " << max_y << "] Z[" << min_z << " - " << max_z
              << "]"
              << "\n";

    // Process faces to extract indices
    for (unsigned i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];

        for (unsigned j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Load texture and shininess from material
    Mesh result(vertices, indices);
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        GLuint texture_id = LoadTextureFromMaterial(scene, material, modelDirectory_);
        result.SetTextureID(texture_id);
        float shininess = GetShininessFromMaterial(material);
        result.SetShininess(shininess);
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
