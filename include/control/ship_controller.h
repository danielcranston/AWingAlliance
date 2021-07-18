#pragma once
#include "control/velocity_controller.h"
#include "control/orientation_controller.h"
#include "geometry.h"

namespace control
{
class ShipController
{
  public:
    ShipController();
    ShipController(const float start_speed);

    geometry::MotionState update(const geometry::MotionState& state,
                                 const Eigen::Isometry3f& target_pose,
                                 const float t,
                                 const float dt);

  private:
    float speed = 0.0f;
    OrientationController orientation_controller;
    VelocityController velocity_controller;
};
}  // namespace control
