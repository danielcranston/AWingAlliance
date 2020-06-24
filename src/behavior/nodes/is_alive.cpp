#include "behavior/nodes/is_alive.h"
#include "actor/ship.h"

IsAlive::IsAlive(const std::string& name,
                 const BT::NodeConfiguration& config,
                 const actor::Ship* ship)
  : BT::ConditionNode(name, config), ship(ship)
{
}

BT::NodeStatus IsAlive::tick()
{
    if (ship->GetHealth() > 0)
        return BT::NodeStatus::SUCCESS;
    else
        return BT::NodeStatus::FAILURE;
};

BT::NodeBuilder IsAlive::Builder(actor::Ship* ship)
{
    return [ship](const std::string& name, const BT::NodeConfiguration& config) {
        return std::make_unique<IsAlive>(name, config, ship);
    };
}
