#pragma once
#include <string>

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/behavior_tree.h>

#include "behavior/nodes/runnable_action_node.h"
#include "spline.h"

namespace actor
{
class Ship;
}

class RoamTowardsDestination : public RunnableActionNode
{
  public:
    RoamTowardsDestination(const std::string& name,
                           const BT::NodeConfiguration& config,
                           actor::Ship* ship,
                           Spline* spline);
    virtual BT::NodeStatus tick() override final;

  private:
    actor::Ship* ship;
    Spline& spline;

    float dt;
    float time_since_start;
};