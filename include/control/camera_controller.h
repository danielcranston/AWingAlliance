#pragma once
#include "control/position_controller.h"
#include "control/orientation_controller.h"

#include "geometry/geometry.h"

namespace control
{
class CameraController
{
  public:
    CameraController();

    geometry::MotionState update(const geometry::MotionState& state,
                                 const Eigen::Isometry3f& target_pose,
                                 const float t,
                                 const float dt);

  private:
    PositionController position_controller;
    OrientationController orientation_controller;
};
}  // namespace control
