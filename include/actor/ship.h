#pragma once

#include <bitset>
#include <chrono>
#include <functional>
#include <map>

#include "actor/actor.h"
#include "actor/laser.h"

#include "behavior/ship_controller.h"

namespace actor
{
enum class Team : int
{
    ALLIANCE,
    EMPIRE
};
const std::map<std::string, actor::Team> team_map = { { "alliance", actor::Team::ALLIANCE },
                                                      { "empire", actor::Team::EMPIRE } };

class Ship : public Actor
{
  public:
    static std::unique_ptr<Ship> Create(const glm::vec3& p,
                                        const glm::vec3& d,
                                        const std::string& name,
                                        const Model* mdl,
                                        const Team team);

    void Update(const float dt) override;
    void Update(const std::bitset<8>& keyboardInfo, float dt);
    void Update_Roaming(float t);
    void Follow(const Actor& target, const float dt);
    void Follow(const float dt);
    bool IsInRange(const glm::vec3& target_pos, const float distance);
    void MoveToLocation(glm::vec3 target_pos, const float dt);
    void Fire();
    bool IsColliding(const Ship& other);
    bool IsColliding(const Laser& laser);
    void TakeDamage(const int amount);

    void SetTarget(Ship* const new_target);
    void ClearTarget();
    void SetDesiredDir(const glm::vec3& new_dir);
    const glm::vec3& GetDesiredDir() const;
    const glm::vec3& GetColor() const;
    const int GetHealth() const;
    const Team& GetTeam() const;
    const float GetSpeed() const;
    const std::string& GetName() const;

    static std::function<void(const Laser& laser)> RegisterLaserFunc;

  private:
    explicit Ship(const glm::vec3& p,
                  const glm::vec3& d,
                  const std::string& name,
                  const Model* mdl,
                  const Team team);

    std::string name;
    glm::vec3 desired_dir;
    float current_speed = 0.0f;
    float current_turnspeed = 0.0f;
    const float max_speed = 50.0f;
    const float max_turnspeed = glm::pi<float>() / 1.5f;

    glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.7f);
    Team team;
    Ship* target = nullptr;

    int health = 100;

    ShipController controller;

    std::chrono::system_clock::time_point last_fired_time;
    std::chrono::system_clock::duration fire_recharge_time = std::chrono::milliseconds(250);
};

}  // namespace actor
