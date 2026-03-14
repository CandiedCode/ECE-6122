#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    GLuint ID;

    Shader(const char *vertPath, const char *fragPath);

    void Use() const;
    void SetMat4(const std::string &name, const glm::mat4 &val) const;
    void SetMat3(const std::string &name, const glm::mat3 &val) const;
    void SetVec3(const std::string &name, const glm::vec3 &val) const;
    void SetFloat(const std::string &name, float val) const;
    void SetInt(const std::string &name, int val) const;
};

#endif