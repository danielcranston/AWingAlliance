#include "behavior/nodes/tumble.h"
#include "actor/ship.h"

Tumble::Tumble(const std::string& name,
               const BT::NodeConfiguration& config,
               actor::Ship* ship,
               const float dt)
  : RunnableActionNode(name, config), ship(ship), dt(dt)
{
}

BT::NodeStatus Tumble::tick()
{
    if (time_since_death == 0.0f)
    {
        dying_dir = ship->GetDirection();
        tumble_duration = 3.0f;
    }

    if (time_since_death < tumble_duration)
    {
        time_since_death += dt;
        float roll = 2.0f * time_since_death * glm::pi<float>();
        ship->SetRoll(roll);
        glm::mat4 anim = glm::rotate(glm::mat4(1.0f), 0.1f * roll, glm::vec3(0, 1, 0)) *
                         glm::rotate(glm::mat4(1.0f), 0.5f * roll, glm::vec3(1, 0, 0));
        ship->SetDirection(glm::mat3(anim) * glm::vec3(0, 0, -1));
        ship->SetPosition(ship->GetPosition() + ship->GetSpeed() * dying_dir * dt);
        return BT::NodeStatus::RUNNING;
    }
    else
    {
        // TODO: Invoke some callback that marks this actor as dead / remove it from the maintained
        // list of ships / spawn some explosion.
        return BT::NodeStatus::SUCCESS;
    }
}

BT::NodeBuilder Tumble::Builder(actor::Ship* ship, const float dt)
{
    return [ship, dt](const std::string& name, const BT::NodeConfiguration& config) {
        return std::make_unique<Tumble>(name, config, ship, dt);
    };
}
