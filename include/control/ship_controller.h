#pragma once
#include "control/velocity_controller.h"
#include "control/orientation_controller.h"
#include "geometry/geometry.h"

namespace control
{
class ShipController
{
  public:
    ShipController();

    geometry::MotionState update(const geometry::MotionState& state,
                                 const Eigen::Isometry3f& target_pose,
                                 const float t,
                                 const float dt);

  private:
    OrientationController orientation_controller;
    VelocityController velocity_controller;
};
}  // namespace control
