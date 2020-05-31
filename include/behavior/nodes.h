#pragma once

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/behavior_tree.h>
#include <glm/glm.hpp>

namespace actor
{
class Ship;
}

class RunnableActionNode : public BT::ActionNodeBase
{
  public:
    RunnableActionNode(const std::string& name, const BT::NodeConfiguration& config);
    virtual ~RunnableActionNode() = default;
    virtual void halt() override final;
};

class IsAlive : public BT::ConditionNode
{
  public:
    IsAlive(const std::string& name, const BT::NodeConfiguration& config, const actor::Ship* ship);
    virtual BT::NodeStatus tick() override final;

  private:
    const actor::Ship* ship;
};

class Tumble : public RunnableActionNode
{
  public:
    Tumble(const std::string& name, const BT::NodeConfiguration& config, actor::Ship* ship);
    virtual BT::NodeStatus tick() override final;

  private:
    actor::Ship* ship;
    glm::vec3 dying_dir;
    float time_since_death = 0.0f;
    float dt;
};
