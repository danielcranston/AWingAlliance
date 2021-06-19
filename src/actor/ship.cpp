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
    auto normalized_actuation = motion_control.get_normalized_actuation();

    ship_controller.set_goal_pose(motion_model.update(normalized_actuation.d_v,
                                                      normalized_actuation.d_w,
                                                      get_position(),
                                                      get_orientation(),
                                                      current_time_s,
                                                      dt));
    ship_controller.update(current_time_s, dt);

    pose = ship_controller.get_pose();

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

Eigen::Isometry3f Ship::get_goal_pose() const
{
    // TODO: move to Actor when Actor::MotionState is complete
    return ship_controller.get_goal_pose();
}

void Ship::toggle_fire_mode()
{
    fire_control.toggle_fire_mode();
}

void Ship::update_input_states(const InputStates& req)
{
    if (req.is_firing.has_value())
    {
        is_firing = *req.is_firing ? true : false;
    }

    if (req.changed_fire_mode)
    {
        fire_control.toggle_fire_mode();
    }

    for (int i = 0; i < req.motion_control_states.size(); ++i)
    {
        const auto& state = req.motion_control_states[i];

        if (state.has_value())
        {
            if (*state)
            {
                motion_control.turn_on(static_cast<control::MotionControl::States>(i));
            }
            else
            {
                motion_control.turn_off(static_cast<control::MotionControl::States>(i));
            }
        }
    }
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
