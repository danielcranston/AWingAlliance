#include <iostream>
#include <memory>
#include "behavior/ship_controller.h"
#include "behavior/nodes.h"
#include "actor/ship.h"

ShipController::ShipController(const actor::Ship* ship)
{
    spline = std::make_unique<Spline>(Spline::Waypoint(glm::vec3(), glm::vec3(), 0.0f),
                                      Spline::Waypoint(glm::vec3(), glm::vec3(), 0.0f));

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
    BT::NodeBuilder builder_SetRoamingDestination =
        [this, non_const_ship](const std::string& name, const BT::NodeConfiguration& config) {
            return std::make_unique<SetRoamingDestination>(
                name, config, non_const_ship, spline.get(), RandomVecFunc);
        };
    BT::NodeBuilder builder_RoamTowardsDestination = [this, non_const_ship](
                                                         const std::string& name,
                                                         const BT::NodeConfiguration& config) {
        return std::make_unique<RoamTowardsDestination>(name, config, non_const_ship, spline.get());
    };

    BT::NodeBuilder builder_FaceTarget = [non_const_ship](const std::string& name,
                                                          const BT::NodeConfiguration& config) {
        return std::make_unique<FaceTarget>(name, config, non_const_ship);
    };

    factory.registerBuilder<IsAlive>("IsAlive", builder_IsAlive);
    factory.registerBuilder<SetTarget>("SetTarget", builder_SetTarget);
    factory.registerBuilder<SetRoamingDestination>("SetRoamingDestination",
                                                   builder_SetRoamingDestination);
    factory.registerBuilder<RoamTowardsDestination>("RoamTowardsDestination",
                                                    builder_RoamTowardsDestination);
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

std::function<glm::vec3(const glm::vec3& center, const glm::vec3& area_size)>
    ShipController::RandomVecFunc =
        [](const glm::vec3& center, const glm::vec3& area_size) -> glm::vec3 {
    // Ideally we'd want something like std::uniform_real_distribution, but this will do for now.
    glm::vec3 random = { std::rand() / (1.0f * RAND_MAX),
                         std::rand() / (1.0f * RAND_MAX),
                         std::rand() / (1.0f * RAND_MAX) };
    return center + (random - 0.5f) * area_size;
};
