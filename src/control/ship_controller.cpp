#include "control/ship_controller.h"

#include <iostream>
#include "geometry/geometry.h"

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
{
}

geometry::MotionState ShipController::update(const geometry::MotionState& state,
                                             const geometry::MotionState& target_state,
                                             const float t,
                                             const float dt)
{
    auto out = orientation_controller.update(state, target_state, dt);
    out = velocity_controller.update(out, target_state, dt);

    return out;
}

}  // namespace control
