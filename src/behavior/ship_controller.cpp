#include <iostream>
#include <memory>
#include "behavior/ship_controller.h"
#include "behavior/nodes.h"
#include "actor/ship.h"

ShipController::ShipController(const actor::Ship* ship)
{
    // Avert your eyes while I cast away const. I want Ship to own its controller, but also for the
    // controller to be able to access the non-const API of Ship so it can update its pose etc..
    actor::Ship* non_const_ship = const_cast<actor::Ship*>(ship);

    BT::BehaviorTreeFactory factory;
    BT::NodeBuilder builder_IsAlive = [non_const_ship](const std::string& name,
                                                       const BT::NodeConfiguration& config) {
        return std::make_unique<IsAlive>(name, config, non_const_ship);
    };
    BT::NodeBuilder builder_Tumble = [non_const_ship](const std::string& name,
                                                      const BT::NodeConfiguration& config) {
        return std::make_unique<Tumble>(name, config, non_const_ship);
    };
    BT::NodeBuilder builder_SetTarget = [non_const_ship](const std::string& name,
                                                         const BT::NodeConfiguration& config) {
        return std::make_unique<SetTarget>(
            name, config, non_const_ship, ShipController::GetTargetFunc);
    };

    BT::NodeBuilder builder_FaceTarget = [non_const_ship](const std::string& name,
                                                          const BT::NodeConfiguration& config) {
        return std::make_unique<FaceTarget>(name, config, non_const_ship);
    };

    factory.registerBuilder<IsAlive>("IsAlive", builder_IsAlive);
    factory.registerBuilder<SetTarget>("SetTarget", builder_SetTarget);
    factory.registerBuilder<FaceTarget>("FaceTarget", builder_FaceTarget);
    factory.registerBuilder<Tumble>("Tumble", builder_Tumble);
    tree = factory.createTreeFromFile("./behavior1.xml");
}

void ShipController::Tick()
{
    tree.tickRoot();
}

std::function<const actor::Ship*(const actor::Ship& requester)> ShipController::GetTargetFunc =
    [](const actor::Ship& requester) -> actor::Ship* {
    throw std::runtime_error("This function needs to be overwritten by the user.");
};
