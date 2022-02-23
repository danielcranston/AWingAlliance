#include "control/camera_controller.h"

namespace control
{
CameraController::CameraController()
{
}

geometry::MotionState CameraController::update(const geometry::MotionState& state,
                                               const Eigen::Isometry3f& target_pose,
                                               const float t,
                                               const float dt)
{
    auto target_state = geometry::MotionState();
    target_state.orientation = Eigen::Quaternionf(target_pose.linear());
    target_state.position = target_pose.translation();

    auto out = state;
    out = orientation_controller.update(out, target_state, dt);
    out = position_controller.update(out, target_state, dt);

    return out;
}
}  // namespace control
