#pragma once

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/behavior_tree.h>
#include <glm/glm.hpp>

#include "behavior/nodes/runnable_action_node.h"

namespace actor
{
class Ship;
}

class Tumble : public RunnableActionNode
{
  public:
    Tumble(const std::string& name, const BT::NodeConfiguration& config, actor::Ship* ship);
    virtual BT::NodeStatus tick() override final;

    static BT::NodeBuilder Builder(actor::Ship* ship);

  private:
    actor::Ship* ship;
    glm::vec3 dying_dir;
    float time_since_death = 0.0f;
    float dt;
};
