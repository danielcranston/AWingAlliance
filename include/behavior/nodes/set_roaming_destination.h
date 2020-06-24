#pragma once

#include <functional>

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/behavior_tree.h>

#include "spline.h"

namespace actor
{
class Ship;
}

class SetRoamingDestination : public BT::SyncActionNode
{
  public:
    SetRoamingDestination(const std::string& name,
                          const BT::NodeConfiguration& config,
                          const actor::Ship* ship,
                          Spline* spline,
                          std::function<glm::vec3(const glm::vec3& center,
                                                  const glm::vec3& area_size)> random_vec_func);
    virtual BT::NodeStatus tick() override final;

  private:
    const actor::Ship* ship;
    Spline& spline;
    std::function<glm::vec3(const glm::vec3& center, const glm::vec3& area_size)> random_vec_func;
};