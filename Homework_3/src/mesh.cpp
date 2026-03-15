#include "mesh.h"
#include <iostream>

static GLuint g_whiteTexture = 0;

GLuint Mesh::GetWhiteTexture()
{
    if (g_whiteTexture == 0)
    {
        unsigned char white[] = {255, 255, 255, 255};
        glGenTextures(1, &g_whiteTexture);
        glBindTexture(GL_TEXTURE_2D, g_whiteTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        std::cout << "Created white fallback texture: " << g_whiteTexture << std::endl;
    }
    return g_whiteTexture;
}

Mesh::~Mesh()
{
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
    if (EBO != 0)
        glDeleteBuffers(1, &EBO);
}

void Mesh::Setup()
{
    static int setupCount = 0;
    setupCount++;
    std::cout << "Mesh::Setup() #" << setupCount << " - vertices: " << vertices.size() << ", indices: " << indices.size()
              << ", current VAO before gen: " << VAO << std::endl;

    glGenVertexArrays(1, &VAO);
    std::cout << "  Generated VAO: " << VAO << " (setupCount=" << setupCount << ")" << std::endl;
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute (layout 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    // Normal attribute (layout 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    // TexCoords attribute (layout 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::Draw() const
{
    glActiveTexture(GL_TEXTURE0);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
        std::cerr << "Error before glBindTexture: " << err << std::endl;

    glBindTexture(GL_TEXTURE_2D, textureID);
    err = glGetError();
    if (err != GL_NO_ERROR)
        std::cerr << "Error after glBindTexture: " << err << std::endl;

    glBindVertexArray(VAO);
    err = glGetError();
    if (err != GL_NO_ERROR)
        std::cerr << "Error after glBindVertexArray: " << err << std::endl;

    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    err = glGetError();
    if (err != GL_NO_ERROR)
        std::cerr << "Error after glDrawElements: " << err << std::endl;

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
