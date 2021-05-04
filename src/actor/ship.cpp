#include <iostream>

#include "actor/ship.h"

namespace actor
{
Ship::Ship(const std::string& name,
           const resources::ActorDescription& description,
           const Eigen::Vector3f& position,
           const Eigen::Quaternionf& orientation,
           const std::string& geometry)
  : Actor(name, position, orientation, description.visual, geometry),
    is_firing(false),
    fire_control(description.primary_fire_control)
{
}

void Ship::tick(float current_time_s, float dt)
{
    if (is_firing)
    {
        auto dispatches = fire_control.fire(current_time_s);

        if (dispatches.has_value())
        {
            for (const auto& offset : dispatches.value())
            {
                auto relative_pose = Eigen::Isometry3f::Identity();
                relative_pose.translation() = offset;
                on_fire_cb(*this, relative_pose);
            }
        }
    }
}

void Ship::toggle_fire_mode()
{
    fire_control.toggle_fire_mode();
}

void Ship::set_on_fire_cb(
    std::function<void(const Ship& ship, const Eigen::Isometry3f relative_pose)> cb)
{
    on_fire_cb = cb;
}

std::function<void(const Ship& ship, const Eigen::Isometry3f relative_pose)> Ship::on_fire_cb =
    [](const Ship& ship, const Eigen::Isometry3f relative_pose) {
        throw std::runtime_error("You need to overwrite Ship::on_fire_cb");
    };
}  // namespace actor