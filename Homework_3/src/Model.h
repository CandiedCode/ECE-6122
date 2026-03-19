/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-03-15
 * Description:  Homework 3: 3D World Scene Rendering with OpenGL and Assimp
 *
 *
 * @file Model.h
 * @brief Header file for loading and managing 3D models using Assimp. The Model class encapsulates the functionality to load a model from a
 * file, process its nodes and meshes, and render it using OpenGL. It uses the Mesh class to represent individual meshes within the model,
 * and the Shader class to handle shader interactions during rendering.
 */

#ifndef HOMEWORK_3_SRC_MODEL_H_
#define HOMEWORK_3_SRC_MODEL_H_

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>
#include <vector>

class Shader;

class Model
{
  public:
    // @brief Constructor that loads a model from the specified file path using Assimp. The constructor reads the model file, processes its
    // nodes and meshes, and stores the resulting Mesh objects in the meshes_ vector. If there is an error during loading, it logs the error
    // message to the console.
    // @param path The file path to the 3D model to be loaded
    explicit Model(const std::string &path);

    // @brief Destructor that cleans up the meshes and their associated OpenGL resources.
    ~Model();

    // @brief Draws the model by calling the Draw() method of each Mesh in the meshes_ vector. This method should be called during the
    // render loop to render the model on the screen. The Draw() method of each Mesh will bind the appropriate OpenGL buffers and issue draw
    // calls to render the mesh geometry.
    void Draw() const;

  private:
    // Vector to store the meshes that make up the model. Each Mesh contains the vertex data, indices, and textures for a part of the model.
    std::vector<Mesh> meshes_;

    // Directory containing the model file, used for resolving relative texture paths
    std::string modelDirectory_;

    // @brief Process a node in the Assimp scene graph recursively.
    // @param node The current node to process
    // @param scene The Assimp scene containing the model data
    void ProcessNode(aiNode *node, const aiScene *scene);

    // @brief Process an Assimp mesh and convert it into a Mesh object that can be rendered with OpenGL.
    // @param mesh The Assimp mesh to process
    // @param scene The Assimp scene containing the model data
    // @return A Mesh object containing the vertex data, indices, and textures for the given Assimp mesh
    auto ProcessMesh(aiMesh *mesh, const aiScene *scene) -> Mesh;
};

#endif // HOMEWORK_3_SRC_MODEL_H_
