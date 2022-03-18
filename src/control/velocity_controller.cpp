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

geometry::MotionState VelocityController::update(const geometry::MotionState& state,
                                                 const geometry::MotionState& goal,
                                                 const float dt)
{
    auto fwd_dir = geometry::get_fwd_dir(state.orientation.toRotationMatrix());

    Eigen::Matrix<float, STATE_DIM, 1> x;
    x << state.velocity.dot(fwd_dir), state.acceleration.dot(fwd_dir);

    Eigen::Matrix<float, STATE_DIM, 1> x_goal;
    x_goal << goal.velocity.dot(geometry::get_fwd_dir(goal.orientation.toRotationMatrix())),
        goal.acceleration.dot(geometry::get_fwd_dir(goal.orientation.toRotationMatrix()));

    auto u = -K * (x - x_goal);
    auto xdot = A * x + B * u;
    x = x + dt * xdot;

    geometry::MotionState out = state;
    out.acceleration = fwd_dir * x.tail<1>();
    out.velocity = fwd_dir * state.velocity.dot(fwd_dir);

    return out;
}
}  // namespace control
