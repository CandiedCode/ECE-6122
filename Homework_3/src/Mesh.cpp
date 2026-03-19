/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-03-15
 * Description:  Homework 3: 3D World Scene Rendering with OpenGL and Assimp
 *
 *
 * @file Mesh.cpp
 * @brief aiMesh for VAO (Vertex Array Object), VBO (Vertex Buffer Object), and EBO (Element Buffer Object).  These objects are created for
 * each aiMesh in the model, and contain the vertex data, index data, and texture ID for that mesh. The Mesh class is responsible for
 * setting up the OpenGL buffers (VAO, VBO, EBO) based on the provided vertex and index data, and for drawing the mesh using glDrawElements.
 * The Mesh class also manages the OpenGL resources by deleting the buffers in the destructor and implementing move semantics to allow
 * efficient transfer of ownership of resources when Mesh objects are moved.
 *
 */

#include "Mesh.h"
#include "Texture.h"
#include <iostream>
#include <string>
#include <vector>

// @brief Helper function to check and report OpenGL errors
// @param errorMsg Descriptive message to prepend to any error output
void CheckGLError(const std::string &errorMsg)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cerr << "Error " << errorMsg << ": " << err << "\n";
    }
}

Mesh::~Mesh()
{
    // Cleanup OpenGL resources
    if (texture_id_ != 0)
    {
        glDeleteTextures(1, &texture_id_);
    }
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
}

void Mesh::Setup()
{
    // Create vertex array object (VAO)
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Create vertex buffer object (VBO)
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices_.size() * sizeof(Vertex)), vertices_.data(), GL_STATIC_DRAW);

    // Create element buffer object (EBO)
    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices_.size() * sizeof(unsigned int)), indices_.data(), GL_STATIC_DRAW);

    // Position attribute - define geometry layout
    glVertexAttribPointer(
        0,              // attribute index 0 in shader
        3,              // size (vec3)
        GL_FLOAT,       // type
        GL_FALSE,       // normalized
        sizeof(Vertex), // stride
        // https://gamedev.stackexchange.com/questions/106141/how-to-correctly-specify-the-offset-in-a-call-to-glvertexattribpointer
        reinterpret_cast<const void *>(
            static_cast<uintptr_t>(offsetof(Vertex, position)))); // NOLINT(performance-no-int-to-ptr) offset in struct,
    glEnableVertexAttribArray(0);

    // Normal attribute - enable lighting calculations in shader
    glVertexAttribPointer(1,              // attribute index 1 in shader
                          3,              // size (vec3)
                          GL_FLOAT,       // type
                          GL_FALSE,       // normalized
                          sizeof(Vertex), // stride
                          reinterpret_cast<const void *>(
                              static_cast<uintptr_t>(offsetof(Vertex, normal)))); // NOLINT(performance-no-int-to-ptr) offset in struct
    glEnableVertexAttribArray(1);

    // Texture Coordinates attribute - enable texture mapping in shader
    glVertexAttribPointer(2,              // attribute index 2 in shader
                          2,              // size (vec2)
                          GL_FLOAT,       // type
                          GL_FALSE,       // normalized
                          sizeof(Vertex), // stride
                          reinterpret_cast<const void *>(static_cast<uintptr_t>(
                              offsetof(Vertex, texture_coords)))); // NOLINT(performance-no-int-to-ptr) offset in struct
    glEnableVertexAttribArray(2);

    // Unbind VAO to prevent accidental modifications
    glBindVertexArray(0);
}

void Mesh::Draw() const
{
    // Activate the texture unit
    glActiveTexture(GL_TEXTURE0);
    CheckGLError("glActiveTexture");

    // Bind the texture for this mesh
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    CheckGLError("glBindTexture");

    // Bind the vertex array object (VAO) for this mesh
    glBindVertexArray(vao_);
    CheckGLError("glBindVertexArray");

    // Draw the mesh using glDrawElements, which uses the EBO for indexed drawing
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, nullptr);
    CheckGLError("glDrawElements");

    // Unbind VAO and texture to clean up state
    glBindVertexArray(0);
    // Unbind the texture to prevent accidental modifications
    glBindTexture(GL_TEXTURE_2D, 0);
}

auto CreateGroundPlane(float size, int subdivisions) -> Mesh
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Calculate the step size for vertex positions based on the desired size and number of subdivisions
    float halfSize = size / 2.0F;
    // The step size determines how far apart the vertices are along the X and Z axes. It is calculated by dividing the total size of the
    // plane by the number of subdivisions, which gives the distance between adjacent vertices in the grid.
    float step = size / static_cast<float>(subdivisions);

    // Generate vertices with tiling UVs
    for (int z = 0; z <= subdivisions; ++z)
    {
        // For each row of vertices along the Z axis, we loop through the columns along the X axis to create a grid of vertices.
        for (int x = 0; x <= subdivisions; ++x)
        {
            Vertex vertex;
            vertex.position = glm::vec3(-halfSize + (static_cast<float>(x) * step), -5.0F, -halfSize + (static_cast<float>(z) * step));
            vertex.normal = glm::vec3(0.0F, 1.0F, 0.0F);

            // Tile the texture 4 times across the plane
            vertex.texture_coords = glm::vec2((static_cast<float>(x) * 4.0F) / static_cast<float>(subdivisions),
                                              (static_cast<float>(z) * 4.0F) / static_cast<float>(subdivisions));
            vertices.push_back(vertex);
        }
    }

    // Generate indices for triangles
    for (int z = 0; z < subdivisions; ++z)
    {
        for (int x = 0; x < subdivisions; ++x)
        {
            int topLeft = (z * (subdivisions + 1)) + x;
            int topRight = topLeft + 1;
            int bottomLeft = ((z + 1) * (subdivisions + 1)) + x;
            int bottomRight = bottomLeft + 1;

            // First triangle
            indices.push_back(static_cast<unsigned int>(topLeft));
            indices.push_back(static_cast<unsigned int>(bottomLeft));
            indices.push_back(static_cast<unsigned int>(topRight));

            // Second triangle
            indices.push_back(static_cast<unsigned int>(topRight));
            indices.push_back(static_cast<unsigned int>(bottomLeft));
            indices.push_back(static_cast<unsigned int>(bottomRight));
        }
    }

    // Create and return the Mesh object for the ground plane using the generated vertices and indices
    return Mesh{vertices, indices};
}
