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
    Q.diagonal() << 1000, 1000, 1000, 1000, 1000, 1000, 1, 1, 1;
    return Q;
}();

static const Eigen::Matrix<float, CONTROL_DIM, CONTROL_DIM> R =
    Eigen::Vector3f(0.1, 0.1, 0.1).asDiagonal();

}  // namespace

PositionController::PositionController(const Eigen::Vector3f& pos) : lqr_controller(A, B, Q, R)
{
    Eigen::Matrix<float, STATE_DIM, 1> x = Eigen::Matrix<float, STATE_DIM, 1>::Zero();
    x.x() = pos.x();
    x.y() = pos.y();
    x.z() = pos.z();
    lqr_controller.set_state(x);
    lqr_controller.set_goal_state(x);
};

void PositionController::update(const float dt)
{
    lqr_controller.update(dt);
}

Eigen::Vector3f PositionController::get_position() const
{
    auto x = lqr_controller.get_state();
    return Eigen::Vector3f(x.x(), x.y(), x.z());
}

Eigen::Vector3f PositionController::get_velocity() const
{
    auto x = lqr_controller.get_state();
    return x.segment<3>(3);
}

void PositionController::set_position(const Eigen::Vector3f& pos)
{
    auto x = lqr_controller.get_state();
    x.x() = pos.x();
    x.y() = pos.y();
    x.z() = pos.z();
    lqr_controller.set_state(x);
}

Eigen::Vector3f PositionController::get_goal_position() const
{
    auto x_goal = lqr_controller.get_goal_state();
    return Eigen::Vector3f(x_goal.x(), x_goal.y(), x_goal.z());
}

void PositionController::set_goal_position(const Eigen::Vector3f& pos)
{
    auto x_goal = lqr_controller.get_goal_state();
    x_goal.x() = pos.x();
    x_goal.y() = pos.y();
    x_goal.z() = pos.z();
    lqr_controller.set_goal_state(x_goal);
}

void PositionController::update_goal_position(const Eigen::Vector3f& relative_pos)
{
    auto x_goal = lqr_controller.get_goal_state();
    x_goal.x() += relative_pos.x();
    x_goal.y() += relative_pos.y();
    x_goal.z() += relative_pos.z();
    lqr_controller.set_goal_state(x_goal);
}

}  // namespace control
