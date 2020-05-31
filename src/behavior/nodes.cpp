#include <behavior/nodes.h>
#include <actor/ship.h>

RunnableActionNode::RunnableActionNode(const std::string& name, const BT::NodeConfiguration& config)
  : BT::ActionNodeBase(name, config)
{
}

void RunnableActionNode::halt()
{
    setStatus(BT::NodeStatus::IDLE);
}

IsAlive::IsAlive(const std::string& name,
                 const BT::NodeConfiguration& config,
                 const actor::Ship* ship)
  : BT::ConditionNode(name, config), ship(ship)
{
}

BT::NodeStatus IsAlive::tick()
{
    if (ship->GetHealth() > 0)
        return BT::NodeStatus::SUCCESS;
    else
        return BT::NodeStatus::FAILURE;
};

Tumble::Tumble(const std::string& name, const BT::NodeConfiguration& config, actor::Ship* ship)
  : RunnableActionNode(name, config), ship(ship), dt(0.001666f)
{
}

BT::NodeStatus Tumble::tick()
{
    if (time_since_death == 0.0f)
        dying_dir = ship->GetDirection();

    time_since_death += dt;
    float roll = 2.0f * time_since_death * glm::pi<float>();
    ship->SetRoll(roll);
    glm::mat4 anim = glm::rotate(glm::mat4(1.0f), 0.1f * roll, glm::vec3(0, 1, 0)) *
                     glm::rotate(glm::mat4(1.0f), 0.5f * roll, glm::vec3(1, 0, 0));
    ship->SetDirection(glm::mat3(anim) * glm::vec3(0, 0, -1));
    ship->SetPosition(ship->GetPosition() + ship->GetSpeed() * dying_dir * dt);

    return BT::NodeStatus::RUNNING;
}
