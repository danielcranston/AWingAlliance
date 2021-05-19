#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "control/lqr.h"

namespace control
{
static constexpr int STATE_DIM = 9;
static constexpr int CONTROL_DIM = 3;

static const Eigen::Matrix<float, STATE_DIM, STATE_DIM> A = []() {
    auto A = Eigen::Matrix<float, STATE_DIM, STATE_DIM>();
    A.row(0) << 0, 0, 0, 1, 0, 0, 0, 0, 0;
    A.row(1) << 0, 0, 0, 0, 1, 0, 0, 0, 0;
    A.row(2) << 0, 0, 0, 0, 0, 1, 0, 0, 0;
    A.row(3) << 0, 0, 0, 0, 0, 0, 1, 0, 0;
    A.row(4) << 0, 0, 0, 0, 0, 0, 0, 1, 0;
    A.row(5) << 0, 0, 0, 0, 0, 0, 0, 0, 1;
    A.row(6) << 0, 0, 0, 0, 0, 0, 1, 0, 0;
    A.row(7) << 0, 0, 0, 0, 0, 0, 0, 1, 0;
    A.row(8) << 0, 0, 0, 0, 0, 0, 0, 0, 1;
    return A;
}();

static const Eigen::Matrix<float, STATE_DIM, CONTROL_DIM> B = []() {
    auto B = Eigen::Matrix<float, STATE_DIM, CONTROL_DIM>();
    B.row(0) << 0, 0, 0;
    B.row(1) << 0, 0, 0;
    B.row(2) << 0, 0, 0;
    B.row(3) << 0, 0, 0;
    B.row(4) << 0, 0, 0;
    B.row(5) << 0, 0, 0;
    B.row(6) << 1, 0, 0;
    B.row(7) << 0, 1, 0;
    B.row(8) << 0, 0, 1;
    return B;
}();

static const Eigen::Matrix<float, STATE_DIM, STATE_DIM> Q = []() {
    auto Q = Eigen::DiagonalMatrix<float, STATE_DIM>();
    Q.diagonal() << 1, 1, 1, 2, 2, 2, 1, 1, 1;
    return Q;
}();

static const Eigen::Matrix<float, CONTROL_DIM, CONTROL_DIM> R =
    Eigen::Vector3f(1, 1, 1).asDiagonal();

class PositionController
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    PositionController(const Eigen::Vector3f& pos);

    void update(const float dt);

    Eigen::Vector3f get_position() const;
    void set_position(const Eigen::Vector3f& pos);
    Eigen::Vector3f get_goal_position();
    void set_goal_position(const Eigen::Vector3f& pos);
    void update_goal_position(const Eigen::Vector3f& relative_pos);

  private:
    LQRController<float, STATE_DIM, CONTROL_DIM> lqr_controller;
};

}  // namespace control
