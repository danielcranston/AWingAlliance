#pragma once

#include <bitset>

#include "actor/actor.h"

namespace actor
{
class Ship : public Actor
{
  public:
    static std::unique_ptr<Actor> Create(const glm::vec3& p, const glm::vec3& d, const Model* mdl);

    void Update(const float dt);
    void Update(const std::bitset<8>& keyboardInfo, float dt);
    void Update_Roaming(float t);

    const glm::vec3& GetDesiredDir();

  private:
    explicit Ship(const glm::vec3& p, const glm::vec3& d, const Model* mdl);

    glm::vec3 desired_dir;
    float current_speed = 0.0f;
    float current_turnspeed = 0.0f;
    const float max_speed = 50.0f;
    const float max_turnspeed = glm::pi<float>() / 1.5f;
};

}  // namespace actor