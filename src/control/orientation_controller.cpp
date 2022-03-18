#include "control/orientation_controller.h"

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace control
{
OrientationController::OrientationController()
  : OrientationController::OrientationController({ 9000.0f, 9000.0f, 9000.0f },
                                                 { 1800.0f, 1800.0f, 1800.0f })
{
}

OrientationController::OrientationController(const Eigen::Vector3f& Kp, const Eigen::Vector3f& Kd)
  : Kp(Kp), Kd(Kd)
{
}

geometry::MotionState OrientationController::update(const geometry::MotionState& state,
                                                    const geometry::MotionState& goal,
                                                    const float dt)
{
    auto out = state;
    const auto& q = state.orientation;

    auto q_err_rotvec = Eigen::AngleAxisf(goal.orientation * q.inverse());
    Eigen::Vector3f q_err = q_err_rotvec.angle() * q_err_rotvec.axis();
    Eigen::Vector3f w_err = goal.angular_velocity - state.angular_velocity;
    Eigen::Vector3f Kp_world = (q * Kp.asDiagonal() * q.inverse()).diagonal();
    Eigen::Vector3f Kd_world = (q * Kd.asDiagonal() * q.inverse()).diagonal();

    out.angular_acceleration = dt * (Kp_world.cwiseProduct(q_err) + Kd_world.cwiseProduct(w_err));

    return out;
}
}  // namespace control
