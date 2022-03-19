#include "control/velocity_controller.h"

#include <iostream>

namespace control
{
namespace
{
constexpr auto STATE_DIM = VelocityController::STATE_DIM;
constexpr auto CONTROL_DIM = VelocityController::CONTROL_DIM;

static const Eigen::Matrix<float, STATE_DIM, STATE_DIM> A = []() {
    auto A = Eigen::Matrix<float, STATE_DIM, STATE_DIM>();
    A << 0, 1, 0, 1;
    return A;
}();

static const Eigen::Matrix<float, STATE_DIM, CONTROL_DIM> B = []() {
    auto B = Eigen::Matrix<float, STATE_DIM, CONTROL_DIM>();
    B << 0, 1;
    return B;
}();

static const Eigen::Matrix<float, STATE_DIM, STATE_DIM> Q = []() {
    auto Q = Eigen::DiagonalMatrix<float, STATE_DIM>();
    Q.diagonal() << 50, 1;
    return Q;
}();

static const Eigen::Matrix<float, CONTROL_DIM, CONTROL_DIM> R = []() {
    auto R = Eigen::DiagonalMatrix<float, CONTROL_DIM>();
    R.diagonal() << 1;
    return R;
}();
}  // namespace

VelocityController::VelocityController() : K(LQR(A, B, Q, R))
{
}

float VelocityController::calculate_dv(const float current_velocity,
                                       const float current_acceleration,
                                       const float goal_velocity,
                                       const float goal_acceleration,
                                       const float dt)
{
    Eigen::Matrix<float, STATE_DIM, 1> x;
    x << current_velocity, current_acceleration;

    Eigen::Matrix<float, STATE_DIM, 1> x_goal;
    x_goal << goal_velocity, goal_acceleration;

    auto u = -K * (x - x_goal);
    auto xdot = A * x + B * u;
    x = x + dt * xdot;

    return x(1);
}
}  // namespace control
