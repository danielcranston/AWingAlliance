#pragma once

#include "actor/actor.h"
#include <functional>
#include <utility>

namespace actor
{
class Camera : public actor::Actor
{
  public:
    explicit Camera();

    static std::function<std::pair<glm::vec3, glm::vec3>()> placement_func;

    void Update(const float dt) override;
    void SetProjMatrix(const glm::mat4& new_projmat);
    const glm::mat4& GetCamMatrix() const;
    const glm::mat4& GetProjMatrix() const;

  private:
    mutable actor::Actor* actor_ptr;

    float fov_y;
    float aspect_ratio;
    float near_distance;
    float far_distance;

    glm::mat4 camMatrix, projMatrix;
};
}  // namespace actor
