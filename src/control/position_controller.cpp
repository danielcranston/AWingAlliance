#include "control/position_controller.h"

namespace control
{
namespace
{
constexpr auto STATE_DIM = PositionController::STATE_DIM;
constexpr auto CONTROL_DIM = PositionController::CONTROL_DIM;

static const Eigen::Matrix<float, STATE_DIM, STATE_DIM> A = []() {
    auto A = Eigen::Matrix<float, STATE_DIM, STATE_DIM>();
    A.row(0) << 0, 0, 0, 1, 0, 0, 0, 0, 0;
    A.row(1) << 0, 0, 0, 0, 1, 0, 0, 0, 0;
    A.row(2) << 0, 0, 0, 0, 0, 1, 0, 0, 0;
    A.row(3) << 0, 0, 0, 0, 0, 0, 10, 0, 0;
    A.row(4) << 0, 0, 0, 0, 0, 0, 0, 10, 0;
    A.row(5) << 0, 0, 0, 0, 0, 0, 0, 0, 10;
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
    Q.diagonal() << 1000, 1000, 1000, 1, 1, 1, 1, 1, 1;
    return Q;
}();

static const Eigen::Matrix<float, CONTROL_DIM, CONTROL_DIM> R =
    Eigen::Vector3f(0.1, 0.1, 0.1).asDiagonal();

}  // namespace

PositionController::PositionController(const Eigen::Vector3f& pos)
  : x(), x_goal(), K(LQR(A, B, Q, R))
{
    x = x.Zero();
    x_goal = x_goal.Zero();
    x.head<3>() = pos;
    x_goal.head<3>() = pos;
};

void PositionController::update(const float dt)
{
    auto u = -K * (x - x_goal);
    auto xdot = A * x + B * u;
    x = x + dt * xdot;
}

Eigen::Vector3f PositionController::get_position() const
{
    return x.head<3>();
}

Eigen::Vector3f PositionController::get_velocity() const
{
    return x.segment<3>(3);
}

void PositionController::set_position(const Eigen::Vector3f& pos)
{
    x.head<3>() = pos;
}

Eigen::Vector3f PositionController::get_goal_position() const
{
    return x_goal.head<3>();
}

void PositionController::set_goal_position(const Eigen::Vector3f& pos)
{
    x_goal.head<3>() = pos;
}
}  // namespace control
