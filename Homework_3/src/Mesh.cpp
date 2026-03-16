/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-03-15
 * Description:  Homework 3: 3D World Scene Rendering with OpenGL and Assimp
 *
 *
 * @file Mesh.cpp
 * @brief aiMesh for VAO (Vertex Array Object), VBO (Vertex Buffer Object), and EBO (Element Buffer Object)
 */

#include "Mesh.h"
#include "Texture.h"
#include <iostream>

Mesh::~Mesh()
{
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
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);

    // Create element buffer object (EBO)
    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);

    // Position attribute (layout 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);

    // Normal attribute (layout 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);

    // TexCoords attribute (layout 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, tex_coords)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::Draw() const
{
    glActiveTexture(GL_TEXTURE0);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cerr << "Error glActiveTexture: " << err << "\n";
    }

    glBindTexture(GL_TEXTURE_2D, texture_id_);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cerr << "Error glBindTexture: " << err << "\n";
    }

    glBindVertexArray(vao_);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cerr << "Error glBindVertexArray: " << err << "\n";
    }

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, nullptr);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cerr << "Error glDrawElements: " << err << "\n";
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
