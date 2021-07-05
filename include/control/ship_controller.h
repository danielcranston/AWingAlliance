#pragma once
#include "control/position_controller.h"
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
};
}  // namespace control
