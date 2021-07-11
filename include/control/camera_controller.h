#pragma once
#include "control/position_controller.h"
#include "control/orientation_controller.h"

#include "geometry.h"

namespace control
{
class CameraController
{
  public:
    CameraController();
    CameraController(const Eigen::Vector3f& start_pos, const Eigen::Quaternionf& start_quat);
    CameraController(const Eigen::Vector3f& start_pos,
                     const Eigen::Quaternionf& start_quat,
                     const float pos_blend_ratio,
                     const float quat_blend_ratio);

    void set_pos_blend_ratio(const float ratio);
    void set_quat_blend_ratio(const float ratio);

    geometry::MotionState update(const geometry::MotionState& state,
                                 const Eigen::Isometry3f& target_pose,
                                 const float t,
                                 const float dt);

  private:
    PositionController position_controller;
    OrientationController orientation_controller;

    float pos_blend_ratio;
    float quat_blend_ratio;
};
}  // namespace control
