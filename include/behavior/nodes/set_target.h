#pragma once

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/behavior_tree.h>

namespace actor
{
class Ship;
}

namespace behavior_nodes
{
class SetTarget : public BT::SyncActionNode
{
  public:
    SetTarget(
        const std::string& name,
        const BT::NodeConfiguration& config,
        const actor::Ship* ship,
        const std::function<const actor::Ship*(const actor::Ship& requester)> get_new_target_func,
        const std::function<void(const actor::Ship*)> set_target_func);
    virtual BT::NodeStatus tick() override final;

    static BT::NodeBuilder
    Builder(actor::Ship* ship,
            std::function<const actor::Ship*(const actor::Ship& requester)> get_new_target_func,
            std::function<void(const actor::Ship*)> set_target_func);

  private:
    const actor::Ship* ship;
    const std::function<const actor::Ship*(const actor::Ship& requester)> get_new_target_func;
    const std::function<void(const actor::Ship*)> set_target_func;
};
}  // namespace behavior_nodes
