#pragma once

#include <glm/glm.hpp>

#include "model.h"

namespace actor
{
class Actor
{
  public:
    static std::unique_ptr<Actor> Create(const glm::vec3& p, const glm::vec3& d, const Model* mdl);

    void SetPosition(const glm::vec3& pos);
    void SetDirection(const glm::vec3& dir);
    void SetPose(const glm::vec3& pos, const glm::vec3& dir);

    const glm::vec3& GetPosition() const;
    const glm::vec3& GetDirection() const;
    const glm::mat4 GetPose() const;

    const Model* GetModel() const;
    const glm::vec3& GetColor() const;

    // These can be pure virtual once I start only making unique ptrs to derived
    virtual void Update(const float dt);
    virtual void PrintInfo() const {};

  protected:
    explicit Actor(const glm::vec3& p, const glm::vec3& d, const Model* mdl);
    glm::vec3 pos;
    glm::vec3 dir;
    const Model* model;
    glm::vec3 color = { 0.0f, 0.0f, 0.7f };
};
}  // namespace actor
