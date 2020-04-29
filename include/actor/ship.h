#pragma once

#include <bitset>
#include <chrono>
#include <functional>

#include "actor/actor.h"
#include "actor/laser.h"

namespace actor
{
class Ship : public Actor
{
  public:
    static std::unique_ptr<Ship> Create(const glm::vec3& p,
                                        const glm::vec3& d,
                                        const Model* mdl,
                                        std::function<void(const Laser& Laser)> laser_func);

    void Update(const float dt) override;
    void Update(const std::bitset<8>& keyboardInfo, float dt);
    void Update_Roaming(float t);
    void Follow(const Actor& target, const float dt);
    void Fire();

    const glm::vec3& GetDesiredDir();

  private:
    explicit Ship(const glm::vec3& p,
                  const glm::vec3& d,
                  const Model* mdl,
                  std::function<void(const Laser& Laser)> laser_func);
    std::function<void(const Laser& laser)> RegisterLaserFunc;

    glm::vec3 desired_dir;
    float current_speed = 0.0f;
    float current_turnspeed = 0.0f;
    const float max_turnspeed = glm::pi<float>() / 1.5f;

    std::chrono::system_clock::time_point last_fired_time;
    std::chrono::system_clock::duration fire_recharge_time = std::chrono::milliseconds(250);
};

}  // namespace actor