#include "behavior/nodes/set_target.h"
#include "actor/ship.h"

SetTarget::SetTarget(
    const std::string& name,
    const BT::NodeConfiguration& config,
    const actor::Ship* ship,
    const std::function<const actor::Ship*(const actor::Ship& requester)> get_target_func)
  : BT::SyncActionNode(name, config), ship(ship), get_target_func(get_target_func)
{
}

BT::PortsList SetTarget::providedPorts()
{
    return { BT::OutputPort<const actor::Ship*>("target") };
}

BT::NodeStatus SetTarget::tick()
{
    BT::TreeNode::setOutput("target", get_target_func(*ship));
    return BT::NodeStatus::SUCCESS;
}

BT::NodeBuilder
SetTarget::Builder(actor::Ship* ship,
                   std::function<const actor::Ship*(const actor::Ship& requester)> get_target_func)
{
    return [ship, get_target_func](const std::string& name, const BT::NodeConfiguration& config) {
        return std::make_unique<SetTarget>(name, config, ship, get_target_func);
    };
}
