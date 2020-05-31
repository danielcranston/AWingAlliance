#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "model.h"

namespace actor
{
class Actor
{
  public:
    void SetPosition(const glm::vec3& pos);
    void SetDirection(const glm::vec3& dir);
    void SetRoll(const float angle);
    void SetPose(const glm::vec3& pos, const glm::vec3& dir);

    const glm::vec3& GetPosition() const;
    const glm::vec3& GetDirection() const;
    const float GetRoll() const;
    const glm::mat4 GetPose() const;

    const Model* GetModel() const;

    virtual void Update(const float dt) = 0;

  protected:
    explicit Actor(const glm::vec3& p, const glm::vec3& d, const Model* mdl);
    glm::vec3 pos;
    glm::vec3 dir;
    float roll = 0.0f;
    const Model* model;
};
}  // namespace actor
