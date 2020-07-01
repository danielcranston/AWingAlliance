#include "behavior/nodes/set_target.h"
#include "actor/ship.h"

namespace behavior_nodes
{
SetTarget::SetTarget(
    const std::string& name,
    const BT::NodeConfiguration& config,
    const actor::Ship* ship,
    const std::function<const actor::Ship*(const actor::Ship& requester)> get_new_target_func,
    std::function<void(const actor::Ship*)> set_target_func)
  : BT::SyncActionNode(name, config),
    ship(ship),
    get_new_target_func(get_new_target_func),
    set_target_func(set_target_func)
{
}

BT::NodeStatus SetTarget::tick()
{
    set_target_func(get_new_target_func(*ship));
    return BT::NodeStatus::SUCCESS;
}

BT::NodeBuilder SetTarget::Builder(
    actor::Ship* ship,
    std::function<const actor::Ship*(const actor::Ship& requester)> get_new_target_func,
    std::function<void(const actor::Ship*)> set_target_func)
{
    return [ship, get_new_target_func, set_target_func](const std::string& name,
                                                        const BT::NodeConfiguration& config) {
        return std::make_unique<SetTarget>(
            name, config, ship, get_new_target_func, set_target_func);
    };
}
}  // namespace behavior_nodes
