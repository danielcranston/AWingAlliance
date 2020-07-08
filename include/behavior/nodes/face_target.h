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
    FaceTarget(const std::string& name,
               const BT::NodeConfiguration& config,
               actor::Ship* ship,
               const float dt,
               const std::function<const actor::Ship*()> get_target_func);
    virtual BT::NodeStatus tick() override final;

    static BT::NodeBuilder Builder(actor::Ship* ship,
                                   const float dt,
                                   const std::function<const actor::Ship*()> get_target_func);

  private:
    actor::Ship* ship;
    float dt;
    float start_time = 0.0f;
    float time_now;
    float max_time = 10.0f;
    const actor::Ship* target;
    const std::function<const actor::Ship*()> get_target_func;
};
