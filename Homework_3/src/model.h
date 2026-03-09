// model.h
#pragma once

#include "mesh.h"
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
    std::vector<Mesh> meshes;
    std::string directory;
    void ProcessNode(aiNode *, const aiScene *);
    Mesh ProcessMesh(aiMesh *, const aiScene *);
};
