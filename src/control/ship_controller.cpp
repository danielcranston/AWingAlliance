#include "control/ship_controller.h"

#include <iostream>
#include "geometry.h"

namespace control
{
namespace
{
constexpr auto STATE_DIM = 3;
constexpr auto CONTROL_DIM = 1;

static const Eigen::Matrix<float, STATE_DIM, STATE_DIM> A = []() {
    auto A = Eigen::Matrix<float, STATE_DIM, STATE_DIM>();
    A.row(0) << 0, 1, 0;
    A.row(1) << 0, 0, 1;
    A.row(2) << 0, 0, 1;
    return A;
}();

static const Eigen::Matrix<float, STATE_DIM, CONTROL_DIM> B = []() {
    auto B = Eigen::Matrix<float, STATE_DIM, CONTROL_DIM>();
    B << 0, 0, 1;
    return B;
}();

static const Eigen::Matrix<float, STATE_DIM, STATE_DIM> Q = []() {
    auto Q = Eigen::DiagonalMatrix<float, STATE_DIM>();
    Q.diagonal() << 1.0f, 1.0f, 1.0f;
    return Q;
}();

static const Eigen::Matrix<float, CONTROL_DIM, CONTROL_DIM> R = Eigen::Matrix<float, 1, 1>(0.1f);

}  // namespace

ShipController::ShipController()
  : ShipController(Eigen::Vector3f::Zero(), 0.0f, Eigen::Quaternionf::Identity())
{
}

ShipController::ShipController(const Eigen::Vector3f& start_pos,
                               const float speed,
                               const Eigen::Quaternionf& start_quat)
  : position(start_pos), speed(speed), orientation_controller(start_quat)
{
}

Eigen::Isometry3f ShipController::get_pose() const
{
    return geometry::make_pose(position, orientation_controller.get_state_quaternion());
}

Eigen::Isometry3f ShipController::get_goal_pose() const
{
    return geometry::make_pose(target_position, orientation_controller.get_goal_quaternion());
}

void ShipController::set_pose(const Eigen::Isometry3f& pose)
{
    orientation_controller.set_state_quaternion(Eigen::Quaternionf(pose.linear()));
    position = pose.translation();
}

void ShipController::set_goal_pose(const Eigen::Isometry3f& target)
{
    orientation_controller.set_goal_quaternion(Eigen::Quaternionf(target.linear()));
    target_position = target.translation();
}

void ShipController::update(const float t, const float dt)
{
    orientation_controller.update(dt);
    speed = (target_position - position)
                .dot(orientation_controller.get_state_quaternion() * -Eigen::Vector3f::UnitZ());
    position +=
        orientation_controller.get_state_quaternion() * Eigen::Vector3f(0.0f, 0.0f, -speed * dt);
}

}  // namespace control
