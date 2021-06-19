#pragma once
#include "control/position_controller.h"
#include "control/orientation_controller.h"

namespace control
{
class ShipController
{
  public:
    ShipController();
    ShipController(const Eigen::Vector3f& start_position,
                   const float start_speed,
                   const Eigen::Quaternionf& start_quat);

    Eigen::Isometry3f get_pose() const;
    Eigen::Isometry3f get_goal_pose() const;

    void set_pose(const Eigen::Isometry3f& pose);
    void set_goal_pose(const Eigen::Isometry3f& target);

    void update(const float t, const float dt);

  private:
    Eigen::Vector3f position;
    Eigen::Vector3f target_position;
    float speed;
    OrientationController orientation_controller;
};
}  // namespace control
