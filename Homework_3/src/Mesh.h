/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-03-15
 * Description:  Homework 3: 3D World Scene Rendering with OpenGL and Assimp
 *
 *
 * @file Mesh.h
 * @brief Header file for VAO (Vertex Array Object), VBO (Vertex Buffer Object), and EBO (Element Buffer Object) for one aiMesh
 */

#ifndef HOMEWORK_3_SRC_MESH_H_
#define HOMEWORK_3_SRC_MESH_H_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <utility>
#include <vector>

// Forward declare to avoid circular dependency
auto GetWhiteTexture() -> GLuint;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

class Mesh
{
  public:
    // @brief constructor initializes VAO, VBO, and EBO to 0
    Mesh() : vao_(0), vbo_(0), ebo_(0)
    {
    }

    // @brief constructor initializes VAO, VBO, EBO, and textureID based on provided vertices and indices, and calls Setup() to create
    // OpenGL buffers
    // @param vertices The list of vertices for this mesh, containing position, normal, and texture coordinate data
    // @param indices The list of indices for this mesh, defining how vertices are connected to form triangles
    // @details The constructor initializes the VAO, VBO, and EBO to 0, sets the textureID to a default white texture, and then calls the
    // Setup() method to create the OpenGL buffers based on the provided vertex and index data.
    Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
        : vao_(0), vbo_(0), ebo_(0), vertices_(vertices), indices_(indices), texture_id_(GetWhiteTexture())
    {
        Setup();
    }

    // @brief Delete copy constructor and copy assignment operator to prevent copying of Mesh objects, which manage OpenGL resources that
    // cannot be safely copied. This ensures that each Mesh instance has unique ownership of its VAO, VBO, EBO, and textureID, and prevents
    // accidental resource leaks or double deletions.
    Mesh(const Mesh &) = delete;
    auto operator=(const Mesh &) -> Mesh & = delete;

    // @brief Move constructor and move assignment operator to allow transfer of ownership of OpenGL resources when a Mesh object is moved.
    // The move constructor and move assignment operator transfer the vertices, indices, VAO, VBO, EBO, and textureID from the source Mesh
    // to the destination Mesh, and then reset the source Mesh's VAO, VBO, EBO, and textureID to 0 to prevent accidental use or deletion of
    // resources that have been transferred.
    Mesh(Mesh &&other) noexcept
        : vao_(other.vao_), vbo_(other.vbo_), ebo_(other.ebo_), vertices_(std::move(other.vertices_)), indices_(std::move(other.indices_)),
          texture_id_(other.texture_id_)
    {
        other.vao_ = 0;
        other.vbo_ = 0;
        other.ebo_ = 0;
        other.texture_id_ = 0;
    }
    auto operator=(Mesh &&other) noexcept -> Mesh &
    {
        // Cleanup old resources
        if (vao_ != 0)
        {
            glDeleteVertexArrays(1, &vao_);
        }
        if (vbo_ != 0)
        {
            glDeleteBuffers(1, &vbo_);
        }
        if (ebo_ != 0)
        {
            glDeleteBuffers(1, &ebo_);
        }

        vertices_ = std::move(other.vertices_);
        indices_ = std::move(other.indices_);
        vao_ = other.vao_;
        vbo_ = other.vbo_;
        ebo_ = other.ebo_;
        texture_id_ = other.texture_id_;

        other.vao_ = 0;
        other.vbo_ = 0;
        other.ebo_ = 0;
        other.texture_id_ = 0;
        return *this;
    }

    // @brief Destructor deletes the OpenGL buffers (VAO, VBO, EBO) if they have been created (i.e., if their IDs are not 0). This ensures
    // that GPU resources are properly released when a Mesh object is destroyed. The destructor checks if each buffer ID is not 0 before
    // calling glDelete* functions to prevent errors from trying to delete uninitialized or already deleted buffers.
    ~Mesh();

    // @brief Draw the mesh by binding the appropriate texture and vertex array, and then issuing a draw call using glDrawElements. The
    // method first activates the texture unit and binds the mesh's texture, then binds the vertex array object (VAO) for this mesh, and
    // finally calls glDrawElements to render the triangles defined by the indices. After drawing, it unbinds the VAO and texture to clean
    // up state.
    void Draw() const;

    // @brief Getters for texture ID
    [[nodiscard]] auto GetTextureID() const -> GLuint
    {
        return texture_id_;
    }
    // @brief Setter for texture ID
    void SetTextureID(GLuint id)
    {
        texture_id_ = id;
    }
    // @brief Getters for vertices
    [[nodiscard]] auto GetVertices() const -> const std::vector<Vertex> &
    {
        return vertices_;
    }
    // @brief Getters for indices
    [[nodiscard]] auto GetIndices() const -> const std::vector<unsigned int> &
    {
        return indices_;
    }

  private:
    // OpenGL buffer IDs
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint ebo_ = 0;

    // Mesh data: list of vertices and indices, and texture ID
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    GLuint texture_id_ = 0;

    // @brief Setup the mesh by creating and binding the VAO, VBO, and EBO, and configuring the vertex attribute pointers. This method
    // generates the OpenGL buffers for the vertex array (VAO), vertex buffer (VBO), and element buffer (EBO), binds them, and uploads the
    // vertex and index data to the GPU. It also sets up the vertex attribute pointers to define how the vertex data is structured
    // (position, normal, texture coordinates) for use in the vertex shader.
    void Setup();
};

#endif // HOMEWORK_3_SRC_MESH_H_
