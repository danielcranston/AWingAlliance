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

ShipController::ShipController() : ShipController(0.0f)
{
}

ShipController::ShipController(const float speed) : speed(speed)
{
}

geometry::MotionState ShipController::update(const geometry::MotionState& state,
                                             const Eigen::Isometry3f& target_pose,
                                             const float t,
                                             const float dt)
{
    auto target_state = geometry::MotionState();
    target_state.orientation = Eigen::Quaternionf(target_pose.linear());

    auto out = orientation_controller.update(state, target_state, dt);

    speed = (target_pose.translation() - state.position)
                .dot(out.orientation * -Eigen::Vector3f::UnitZ());
    out.velocity = geometry::get_fwd_dir(out.orientation.toRotationMatrix()) * speed;
    out.position += out.velocity * dt;

    return out;
}

}  // namespace control
