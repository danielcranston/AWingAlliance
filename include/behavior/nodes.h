#pragma once
#include <string>

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/behavior_tree.h>
#include <glm/glm.hpp>

#include "spline.h"

namespace actor
{
class Ship;
}

class ShipController;

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

class FaceTarget : public RunnableActionNode
{
  public:
    FaceTarget(const std::string& name, const BT::NodeConfiguration& config, actor::Ship* ship);
    static BT::PortsList providedPorts();
    virtual BT::NodeStatus tick() override final;

  private:
    actor::Ship* ship;
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
