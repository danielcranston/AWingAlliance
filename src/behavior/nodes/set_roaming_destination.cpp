#include <iomanip>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "behavior/nodes/set_roaming_destination.h"
#include "actor/ship.h"

SetRoamingDestination::SetRoamingDestination(
    const std::string& name,
    const BT::NodeConfiguration& config,
    const actor::Ship* ship,
    Spline* spline,
    std::function<glm::vec3(const glm::vec3& center, const glm::vec3& area_size)> random_vec_func)
  : BT::SyncActionNode(name, config), ship(ship), spline(*spline), random_vec_func(random_vec_func)
{
}

BT::NodeStatus SetRoamingDestination::tick()
{
    // We start roaming from where we are right now
    const Spline::Waypoint start(ship->GetPosition(), ship->GetDirection(), ship->GetSpeed());

    // The destination will be kind of arbitrary for now (same for final direction..)
    const glm::vec3 destination_center = { 0.0f, 128.0f, 0.0f };
    const glm::vec3 destination_volume_size = { 512.0f, 32.0f, 512.0f };
    const glm::vec3 random_destination =
        random_vec_func(destination_center, destination_volume_size);
    const glm::vec3 random_direction = random_vec_func(glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f));
    const Spline::Waypoint end(
        random_destination, glm::normalize(random_direction), ship->GetSpeed());

    spline = Spline(start, end);

    if (ship->GetName() == "awing2")
    {
        std::cout << std::fixed << std::setprecision(3);
        std::cout << ship->GetName()
                  << " SetRoamingDestination: " << glm::to_string(random_destination) << std::endl;
    }

    return BT::NodeStatus::SUCCESS;
}

BT::NodeBuilder SetRoamingDestination::Builder(
    actor::Ship* ship,
    Spline* spline,
    std::function<glm::vec3(const glm::vec3& center, const glm::vec3& area_size)> random_vec_func)
{
    return [ship, spline, random_vec_func](const std::string& name,
                                           const BT::NodeConfiguration& config) {
        return std::make_unique<SetRoamingDestination>(name, config, ship, spline, random_vec_func);
    };
}
