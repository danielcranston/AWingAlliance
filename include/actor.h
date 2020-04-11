#pragma once

#include <vector>
#include <map>
#include <memory>
#include <bitset>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include <model.h>

namespace actor
{
class Actor
{
  public:
    Actor(const glm::vec3& p, const glm::vec3& d, const Model* mdl);

    virtual void Draw(glm::mat4 camprojMat);
    void SetPosition(glm::vec3 pos);
    void SetDirection(glm::vec3 dir);
    void SetOrientation(glm::vec3 pos, glm::vec3 dir);

    glm::vec3 pos;
    glm::vec3 dir;
    const Model* model;

  protected:
    glm::mat4 mdlMatrix;
};
}  // namespace actor
