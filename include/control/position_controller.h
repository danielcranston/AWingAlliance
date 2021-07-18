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

    geometry::MotionState update(const geometry::MotionState& state,
                                 const geometry::MotionState& goal,
                                 const float dt);

  private:
    Eigen::Matrix<float, CONTROL_DIM, STATE_DIM> K;
};

}  // namespace control
