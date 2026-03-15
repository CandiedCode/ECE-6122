#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <utility>
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
        : vertices(verts), indices(inds), VAO(0), VBO(0), EBO(0), textureID(GetWhiteTexture())
    {
        Setup();
    }

    // Prevent copying (VAO/VBO/EBO are not copyable)
    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    // Allow moving
    Mesh(Mesh &&other) noexcept
        : vertices(std::move(other.vertices)), indices(std::move(other.indices)), VAO(other.VAO), VBO(other.VBO), EBO(other.EBO),
          textureID(other.textureID)
    {
        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
        other.textureID = 0;
    }

    Mesh &operator=(Mesh &&other) noexcept
    {
        // Cleanup old resources
        if (VAO != 0)
            glDeleteVertexArrays(1, &VAO);
        if (VBO != 0)
            glDeleteBuffers(1, &VBO);
        if (EBO != 0)
            glDeleteBuffers(1, &EBO);

        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        textureID = other.textureID;

        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
        other.textureID = 0;
        return *this;
    }

    ~Mesh();
    void Draw() const;

    static GLuint GetWhiteTexture();

  private:
    GLuint VAO, VBO, EBO;
    void Setup();
};
