#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "geometry/geometry.h"

namespace control
{
class OrientationController
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    OrientationController();
    OrientationController(const Eigen::Vector3f& Kp, const Eigen::Vector3f& Kd);

    Eigen::Vector3f calculate_dw(const Eigen::Quaternionf& current_orientation,
                                 const Eigen::Vector3f& current_angular_velocity,
                                 const Eigen::Quaternionf& goal_orientation,
                                 const Eigen::Vector3f& goal_angular_velocity,
                                 const float dt);

  private:
    Eigen::Vector3f Kp;  // Proportional controller gains
    Eigen::Vector3f Kd;  // Derivative controller gains
};
}  // namespace control
