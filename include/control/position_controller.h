#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "control/lqr.h"

namespace control
{
class PositionController
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    static constexpr int STATE_DIM = 9;
    static constexpr int CONTROL_DIM = 3;

    PositionController(const Eigen::Vector3f& pos);

    void update(const float dt);

    Eigen::Vector3f get_position() const;
    Eigen::Vector3f get_velocity() const;
    void set_position(const Eigen::Vector3f& pos);
    Eigen::Vector3f get_goal_position() const;
    void set_goal_position(const Eigen::Vector3f& pos);
    void update_goal_position(const Eigen::Vector3f& relative_pos);

  private:
    Eigen::Matrix<float, STATE_DIM, 1> x;
    Eigen::Matrix<float, STATE_DIM, 1> x_goal;
    Eigen::Matrix<float, CONTROL_DIM, STATE_DIM> K;
};

}  // namespace control
