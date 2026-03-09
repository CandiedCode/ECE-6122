#pragma once

#include <glm/glm.hpp>

class Shader
{
  public:
    Shader() = default;

    void SetMat4(const char *name, const glm::mat4 &value) const
    {
        // TODO: Implement glUniformMatrix4fv call
    }

    void SetMat3(const char *name, const glm::mat3 &value) const
    {
        // TODO: Implement glUniformMatrix3fv call
    }

    void SetFloat(const char *name, float value) const
    {
        // TODO: Implement glUniform1f call
    }
};
