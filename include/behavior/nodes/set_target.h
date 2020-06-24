#pragma once

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/behavior_tree.h>

namespace actor
{
class Ship;
}

class SetTarget : public BT::SyncActionNode
{
  public:
    SetTarget(
        const std::string& name,
        const BT::NodeConfiguration& config,
        const actor::Ship* ship,
        const std::function<const actor::Ship*(const actor::Ship& requester)> get_target_func);
    static BT::PortsList providedPorts();
    virtual BT::NodeStatus tick() override final;

  private:
    const actor::Ship* ship;
    const std::function<const actor::Ship*(const actor::Ship& requester)> get_target_func;
};