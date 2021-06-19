#include "control/camera_controller.h"

#include "geometry.h"

namespace control
{
CameraController::CameraController()
  : CameraController(Eigen::Vector3f::Zero(), Eigen::Quaternionf::Identity())
{
}

CameraController::CameraController(const Eigen::Vector3f& start_pos,
                                   const Eigen::Quaternionf& start_quat)
  : CameraController::CameraController(start_pos, start_quat, 0.5f, 0.8f)
{
}

CameraController::CameraController(const Eigen::Vector3f& start_pos,
                                   const Eigen::Quaternionf& start_quat,
                                   const float pos_blend_ratio,
                                   const float quat_blend_ratio)
  : position_controller(start_pos),
    orientation_controller(start_quat),
    pos_blend_ratio(pos_blend_ratio),
    quat_blend_ratio(quat_blend_ratio)
{
}

void CameraController::set_target_pose(const Eigen::Isometry3f& target)
{
    orientation_controller.set_goal_quaternion(Eigen::Quaternionf(target.linear()));
    position_controller.set_goal_position(target.translation());
}

void CameraController::set_pos_blend_ratio(const float ratio)
{
    pos_blend_ratio = ratio;
}
void CameraController::set_quat_blend_ratio(const float ratio)
{
    quat_blend_ratio = ratio;
}

void CameraController::update(const float t, const float dt)
{
    orientation_controller.update(dt);
    position_controller.update(dt);
}

Eigen::Isometry3f CameraController::get_pose() const
{
    // To save the hassle of tuning the controllers perfectly, (s)lerp between controller outputs
    // and desired pose for convenience.
    Eigen::Isometry3f out(geometry::slerp(orientation_controller.get_state_quaternion(),
                                          orientation_controller.get_goal_quaternion(),
                                          quat_blend_ratio));
    out.translation() = geometry::lerp(position_controller.get_position(),
                                       position_controller.get_goal_position(),
                                       pos_blend_ratio);

    return out;
}

}  // namespace control
