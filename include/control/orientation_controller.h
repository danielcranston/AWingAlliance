#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "geometry.h"

namespace control
{
class OrientationController
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    OrientationController();
    OrientationController(const Eigen::Vector3f& Kp,
                          const Eigen::Vector3f& Kd);  //@TODO: Should add max torque/w

    geometry::MotionState update(const geometry::MotionState& state,
                                 const geometry::MotionState& goal,
                                 const float dt);

  private:
    Eigen::Vector3f Kp;  // Proportional controller gains
    Eigen::Vector3f Kd;  // Derivative controller gains
};
}  // namespace control
