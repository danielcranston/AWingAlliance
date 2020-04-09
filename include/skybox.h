#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <model.h>

class Skybox
{
  public:
    explicit Skybox(Model* mdl, unsigned int tex_id, unsigned int program);
    void Draw(const glm::mat4& projMatrix, const glm::mat4& camMatrix);
    void LoadAndSetTexture(std::string folder);
    glm::mat4 no_translation;
    Model* model;
    unsigned int texture_id;

  private:
    unsigned int program;
};