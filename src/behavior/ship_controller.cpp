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

    factory.registerBuilder<IsAlive>("IsAlive", IsAlive::Builder(non_const_ship));

    std::function<void(const actor::Ship*)> set_target_func = [this](const actor::Ship* new_ship) {
        this->SetTarget(new_ship);
    };

    BT::NodeBuilder builder =
        behavior_nodes::SetTarget::Builder(non_const_ship, GetNewTargetFunc, set_target_func);
    factory.registerBuilder<behavior_nodes::SetTarget>("SetTarget", builder);
    factory.registerBuilder<SetRoamingDestination>(
        "SetRoamingDestination",
        SetRoamingDestination::Builder(non_const_ship, spline.get(), RandomVecFunc));
    factory.registerBuilder<RoamTowardsDestination>(
        "RoamTowardsDestination",
        RoamTowardsDestination::Builder(non_const_ship, GetTimeStepFunc(), spline.get()));
    factory.registerBuilder<FaceTarget>(
        "FaceTarget",
        FaceTarget::Builder(
            non_const_ship, GetTimeStepFunc(), std::bind(&ShipController::GetTarget, this)));
    factory.registerBuilder<Tumble>("Tumble", Tumble::Builder(non_const_ship, GetTimeStepFunc()));
    tree = factory.createTreeFromFile("./behavior1.xml");
}

void ShipController::Tick()
{
    tree.tickRoot();
}

const actor::Ship* ShipController::GetTarget()
{
    return target;
}

void ShipController::SetTarget(const actor::Ship* new_target)
{
    target = new_target;
}

std::function<const actor::Ship*(const actor::Ship& requester)> ShipController::GetNewTargetFunc =
    [](const actor::Ship& requester) -> actor::Ship* {
    throw std::runtime_error("This function needs to be overwritten by the user.");
};

std::function<glm::vec3(const glm::vec3& center, const glm::vec3& area_size)>
    ShipController::RandomVecFunc =
        [](const glm::vec3& center, const glm::vec3& area_size) -> glm::vec3 {
    // Ideally we'd want something like std::uniform_real_distribution, but this will do for
    // now.
    glm::vec3 random = { std::rand() / (1.0f * RAND_MAX),
                         std::rand() / (1.0f * RAND_MAX),
                         std::rand() / (1.0f * RAND_MAX) };
    return center + (random - 0.5f) * area_size;
};

std::function<float()> ShipController::GetTimeStepFunc = []() -> float {
    throw std::runtime_error("ShipController::GetTimeStepFunc needs to be overwritten by the "
                             "user.");
};
