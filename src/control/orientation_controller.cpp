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

Eigen::Vector3f OrientationController::calculate_dw(const Eigen::Quaternionf& current_orientation,
                                                    const Eigen::Vector3f& current_angular_velocity,
                                                    const Eigen::Quaternionf& goal_orientation,
                                                    const Eigen::Vector3f& goal_angular_velocity,
                                                    const float dt)
{
    const auto& q = current_orientation;

    auto q_err_rotvec = Eigen::AngleAxisf(goal_orientation * q.inverse());
    Eigen::Vector3f q_err = q_err_rotvec.angle() * q_err_rotvec.axis();
    Eigen::Vector3f w_err = goal_angular_velocity - current_angular_velocity;
    Eigen::Vector3f Kp_world = (q * Kp.asDiagonal() * q.inverse()).diagonal();
    Eigen::Vector3f Kd_world = (q * Kd.asDiagonal() * q.inverse()).diagonal();

    return dt * (Kp_world.cwiseProduct(q_err) + Kd_world.cwiseProduct(w_err));
}
}  // namespace control
