#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "geometry/geometry.h"
#include "control/lqr.h"

namespace control
{
class PositionController
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    static constexpr int STATE_DIM = 9;
    static constexpr int CONTROL_DIM = 3;

    PositionController();

    Eigen::Vector3f calculate_dv(const Eigen::Vector3f& current_position,
                                 const Eigen::Vector3f& current_velocity,
                                 const Eigen::Vector3f& current_acceleration,
                                 const Eigen::Vector3f& goal_position,
                                 const Eigen::Vector3f& goal_velocity,
                                 const Eigen::Vector3f& goal_acceleration,
                                 const float dt);

  private:
    Eigen::Matrix<float, CONTROL_DIM, STATE_DIM> K;
};

}  // namespace control
