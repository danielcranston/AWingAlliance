#pragma once
#include "control/position_controller.h"
#include "control/orientation_controller.h"

namespace control
{
class CameraController
{
  public:
    CameraController();

    void set_target_pose(Eigen::Isometry3f& target);

    void update(const float t, const float dt);

    Eigen::Isometry3f get_pose() const;

  private:
    PositionController position_controller;
    OrientationController orientation_controller;
};
}  // namespace control