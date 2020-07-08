#include <iomanip>

#include "behavior/nodes/roam_towards_destination.h"
#include "actor/ship.h"

RoamTowardsDestination::RoamTowardsDestination(const std::string& name,
                                               const BT::NodeConfiguration& config,
                                               actor::Ship* ship,
                                               const float dt,
                                               Spline* spline)
  : RunnableActionNode(name, config), ship(ship), spline(*spline), dt(dt), time_since_start(0.0f)
{
}

BT::NodeStatus RoamTowardsDestination::tick()
{
    float travel_duration = spline.GetTravelDuration();

    float u = std::min(1.0f, time_since_start / travel_duration);
    glm::vec3 pos = spline(u).first;

    ship->SetPose(pos, glm::normalize(pos - ship->GetPosition()));

    if (time_since_start <= travel_duration)
    {
        time_since_start += dt;
        return BT::NodeStatus::RUNNING;
    }
    else
    {
        time_since_start = 0.0f;
        return BT::NodeStatus::SUCCESS;
    }
}

BT::NodeBuilder RoamTowardsDestination::Builder(actor::Ship* ship, const float dt, Spline* spline)
{
    return [ship, dt, spline](const std::string& name, const BT::NodeConfiguration& config) {
        return std::make_unique<RoamTowardsDestination>(name, config, ship, dt, spline);
    };
}
