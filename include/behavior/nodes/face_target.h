#pragma once

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/behavior_tree.h>

#include "behavior/nodes/runnable_action_node.h"

namespace actor
{
class Ship;
}

class FaceTarget : public RunnableActionNode
{
  public:
    FaceTarget(const std::string& name, const BT::NodeConfiguration& config, actor::Ship* ship);
    static BT::PortsList providedPorts();
    virtual BT::NodeStatus tick() override final;

  private:
    actor::Ship* ship;
};