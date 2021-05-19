#include "control/position_controller.h"

namespace control
{
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

void PositionController::set_position(const Eigen::Vector3f& pos)
{
    auto x = lqr_controller.get_state();
    x.x() = pos.x();
    x.y() = pos.y();
    x.z() = pos.z();
    lqr_controller.set_state(x);
}

Eigen::Vector3f PositionController::get_goal_position()
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
