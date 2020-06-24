#pragma once

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/behavior_tree.h>

namespace actor
{
class Ship;
}

class IsAlive : public BT::ConditionNode
{
  public:
    IsAlive(const std::string& name, const BT::NodeConfiguration& config, const actor::Ship* ship);
    virtual BT::NodeStatus tick() override final;

  private:
    const actor::Ship* ship;
};
