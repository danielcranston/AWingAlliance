#pragma once

#include <glm/glm.hpp>

#include <model.h>

class Skybox
{
  public:
    explicit Skybox(const unsigned int vao, unsigned int tex_id, unsigned int program);
    void Draw(const glm::mat4& projMatrix, const glm::mat4& camMatrix) const;

  private:
    const unsigned int vao;
    const unsigned int program;
    const unsigned int texture_id;
    mutable glm::mat4 no_translation;
};