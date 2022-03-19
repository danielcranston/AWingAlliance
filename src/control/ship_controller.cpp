#include "control/ship_controller.h"

#include <iostream>
#include "geometry/geometry.h"

namespace control
{
ShipController::ShipController()
{
}

geometry::MotionState ShipController::update(const geometry::MotionState& state,
                                             const geometry::MotionState& target_state,
                                             const float dt)
{
    Eigen::Vector3f d_w = orientation_controller.calculate_dw(state.orientation,
                                                              state.angular_velocity,
                                                              target_state.orientation,
                                                              target_state.angular_velocity,
                                                              dt);

    auto fwd_dir = geometry::get_fwd_dir(state.orientation.toRotationMatrix());
    auto target_fwd_dir = geometry::get_fwd_dir(target_state.orientation.toRotationMatrix());

    float d_v = velocity_controller.calculate_dv(state.velocity.dot(fwd_dir),
                                                 state.acceleration.dot(fwd_dir),
                                                 target_state.velocity.dot(target_fwd_dir),
                                                 target_state.acceleration.dot(target_fwd_dir),
                                                 dt);

    auto out = state;
    out.angular_acceleration = d_w;
    out.acceleration = d_v * fwd_dir;
    out.velocity = fwd_dir * state.velocity.dot(fwd_dir);
    return out;
}

}  // namespace control
