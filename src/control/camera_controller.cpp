#include "control/camera_controller.h"

namespace control
{
CameraController::CameraController()
  : CameraController(Eigen::Vector3f::Zero(), Eigen::Quaternionf::Identity())
{
}

CameraController::CameraController(const Eigen::Vector3f& start_pos,
                                   const Eigen::Quaternionf& start_quat)
  : CameraController::CameraController(start_pos, start_quat, 0.1f, 0.1f)
{
}

CameraController::CameraController(const Eigen::Vector3f& start_pos,
                                   const Eigen::Quaternionf& start_quat,
                                   const float pos_blend_ratio,
                                   const float quat_blend_ratio)
  : position_controller(start_pos),
    orientation_controller(),
    pos_blend_ratio(pos_blend_ratio),
    quat_blend_ratio(quat_blend_ratio)
{
}

void CameraController::set_pos_blend_ratio(const float ratio)
{
    pos_blend_ratio = ratio;
}
void CameraController::set_quat_blend_ratio(const float ratio)
{
    quat_blend_ratio = ratio;
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

    // To save the hassle of tuning the controllers perfectly, (s)lerp between controller outputs
    // and desired pose for convenience.
    out.orientation = geometry::slerp(out.orientation, target_state.orientation, quat_blend_ratio);
    out.position = geometry::lerp(out.position, target_state.position, pos_blend_ratio);

    return out;
}
}  // namespace control
