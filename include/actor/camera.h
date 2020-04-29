#pragma once

#include "actor/actor.h"

class Camera : public actor::Actor
{
  public:
    explicit Camera();

    void attach_to(actor::Actor* new_ptr) const;

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
