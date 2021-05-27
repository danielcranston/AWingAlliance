#include "control/orientation_controller.h"

#include "geometry.h"

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace control
{
OrientationController::State::State(const Eigen::Quaternionf& q, const Eigen::Vector3f& w)
  : q(q), w(w)
{
}

OrientationController::OrientationController()
  : OrientationController::OrientationController(Eigen::Quaternionf::Identity())
{
}

OrientationController::OrientationController(const Eigen::Quaternionf& start_quat)
  : OrientationController::OrientationController({ start_quat, { 0.0f, 0.0f, 0.0f } },
                                                 { start_quat, { 0.0f, 0.0f, 0.0f } },
                                                 { 150.0f, 150.0f, 150.0f },
                                                 { 30.0f, 30.0f, 30.0f },
                                                 Eigen::Matrix3f::Identity())
{
}

OrientationController::OrientationController(const OrientationController::State& state,
                                             const OrientationController::State& goal,
                                             const Eigen::Vector3f& Kp,
                                             const Eigen::Vector3f& Kd,
                                             const Eigen::Matrix3f& inertia_matrix)
  //@TODO: Should add max torque/w
  : state(state), goal(goal), Kp(Kp), Kd(Kd), inertia_matrix(inertia_matrix){};

void OrientationController::set_state(const Eigen::Quaternionf& q, const Eigen::Vector3f& w)
{
    state.q = q;
    state.w = w;
}

void OrientationController::set_state_quaternion(const Eigen::Quaternionf& q)
{
    state.q = q;
}

void OrientationController::set_goal(const Eigen::Quaternionf& q, const Eigen::Vector3f& w)
{
    goal.q = q;
    goal.w = w;
}

void OrientationController::set_goal_quaternion(const Eigen::Quaternionf& q)
{
    goal.q = q;
}

void OrientationController::update_goal_quaternion(const Eigen::Quaternionf& relative_q)
{
    goal.q = goal.q * relative_q;
}

OrientationController::State OrientationController::get_state() const
{
    return state;
}

Eigen::Quaternionf OrientationController::get_state_quaternion() const
{
    return state.q;
}

OrientationController::State OrientationController::get_goal() const
{
    return goal;
}

Eigen::Quaternionf OrientationController::get_goal_quaternion() const
{
    return goal.q;
}

void OrientationController::update(const float dt)
{
    // Calculate necessary quantities
    Eigen::Matrix3f inertia_world = state.q * inertia_matrix * state.q.inverse();
    angular_momentum = inertia_world * state.w;
    auto w_body = state.q.inverse() * state.w;
    auto dq = geometry::angular_velocity_to_quat(state.w, dt);

    // Compute PD Control (torque)
    Eigen::AngleAxisf q_err_rotvec;
    q_err_rotvec = goal.q * state.q.inverse();
    Eigen::Vector3f q_err = q_err_rotvec.angle() * q_err_rotvec.axis();
    Eigen::Vector3f w_err = goal.w - state.w;
    Eigen::Vector3f Kp_world = (state.q * Kp.asDiagonal() * state.q.inverse()).diagonal();
    Eigen::Vector3f Kd_world = (state.q * Kd.asDiagonal() * state.q.inverse()).diagonal();
    Eigen::Vector3f torque = Kp_world.cwiseProduct(q_err) + Kd_world.cwiseProduct(w_err);

    // Update state
    state.q = (dq * state.q);
    auto R = state.q.toRotationMatrix();
    inertia_world = R * inertia_matrix * R.transpose();
    angular_momentum = angular_momentum + torque * dt;
    state.w = inertia_world.inverse() * angular_momentum;
}
}  // namespace control
