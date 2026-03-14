#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh
{
  public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GLuint textureID = 0;

    Mesh() = default;
    Mesh(const std::vector<Vertex> &verts, const std::vector<unsigned int> &inds)
        : vertices(verts), indices(inds), VAO(0), VBO(0), EBO(0)
    {
        Setup();
    }

    ~Mesh();
    void Draw() const;

  private:
    GLuint VAO, VBO, EBO;
    void Setup();
};
