#pragma once

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

  private:
    explicit Ship(const glm::vec3& p, const glm::vec3& d, const Model* mdl);

    float current_speed = 0.0f;
    float current_turnspeed = 0.0f;
    const float max_speed = 10.0f;
    const float max_turnspeed = glm::pi<float>() / 5.0f;
};

}  // namespace actor