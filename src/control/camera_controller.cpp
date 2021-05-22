#include "control/camera_controller.h"

namespace control
{
CameraController::CameraController()
  : position_controller({ 0.0f, 0.0f, 0.0f }),
    orientation_controller({ Eigen::Quaternionf::Identity(), { 0.0f, 0.0f, 0.0f } },
                           { Eigen::Quaternionf::Identity(), { 0.0f, 0.0f, 0.0f } },
                           { 150.0f, 150.0f, 150.0f },
                           { 30.0f, 30.0f, 30.0f },
                           Eigen::Matrix3f::Identity())
{
}

void CameraController::set_target_pose(Eigen::Isometry3f& target)
{
    orientation_controller.set_goal_quaternion(Eigen::Quaternionf(target.linear()));
    position_controller.set_goal_position(target.translation());
}

void CameraController::update(const float t, const float dt)
{
    orientation_controller.update(dt);
    position_controller.update(dt);
}

Eigen::Isometry3f CameraController::get_pose() const
{
    Eigen::Isometry3f out(orientation_controller.get_state_quaternion());
    out.translation() = position_controller.get_position();
    return out;
}

}  // namespace control