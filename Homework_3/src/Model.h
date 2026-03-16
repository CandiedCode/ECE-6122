/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-03-15
 * Description:  Homework 3: 3D World Scene Rendering with OpenGL and Assimp
 *
 *
 * @file Model.h
 * @brief Header file for loading and managing 3D models using Assimp
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
    explicit Model(const std::string &path);
    void Draw(Shader &shader) const;

  private:
    std::vector<Mesh> meshes_;
    std::string directory_;
    void ProcessNode(aiNode *node, const aiScene *scene);
    static auto ProcessMesh(aiMesh *mesh, const aiScene *scene) -> Mesh;
};

#endif // HOMEWORK_3_SRC_MODEL_H_
