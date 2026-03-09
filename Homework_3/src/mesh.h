#pragma once

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

    Mesh() = default;
    Mesh(const std::vector<Vertex> &verts, const std::vector<unsigned int> &inds) : vertices(verts), indices(inds)
    {
    }

    void Draw() const
    {
        // TODO: Implement OpenGL rendering
    }
};
