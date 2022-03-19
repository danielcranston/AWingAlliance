#include "control/camera_controller.h"

namespace control
{
CameraController::CameraController()
{
}

geometry::MotionState CameraController::update(const geometry::MotionState& state,
                                               const geometry::MotionState& target_state,
                                               const float dt)
{
    Eigen::Vector3f d_w = orientation_controller.calculate_dw(state.orientation,
                                                              state.angular_velocity,
                                                              target_state.orientation,
                                                              target_state.angular_velocity,
                                                              dt);
    Eigen::Vector3f d_v = position_controller.calculate_dv(state.position,
                                                           state.velocity,
                                                           state.acceleration,
                                                           target_state.position,
                                                           target_state.velocity,
                                                           target_state.acceleration,
                                                           dt);

    auto out = state;
    out.angular_acceleration = d_w;
    out.acceleration = d_v;
    return out;
}
}  // namespace control
