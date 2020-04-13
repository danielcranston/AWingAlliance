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
    static std::unique_ptr<Actor> Create(const glm::vec3& p, const glm::vec3& d, const Model* mdl);
    Actor(const glm::vec3& p, const glm::vec3& d, const Model* mdl);

    const Model* GetModel() const;
    void SetPosition(glm::vec3 pos);
    void SetDirection(glm::vec3 dir);
    void SetOrientation(glm::vec3 pos, glm::vec3 dir);
    virtual void Dunny(){};  // Need at least one virtual function for dynamic casts, and I
                             // couldn't think of a meaningful one right now...

    glm::vec3 pos;
    glm::vec3 dir;
    const Model* model;

  protected:
    glm::mat4 mdlMatrix;
};
}  // namespace actor
