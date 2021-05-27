#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace control
{
class OrientationController
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    struct State
    {
        State(const Eigen::Quaternionf& q, const Eigen::Vector3f& w);

        Eigen::Quaternionf q;  // representing rotation from body to world frame
        Eigen::Vector3f w;     // angular velocity in world frame
    };

    OrientationController();
    OrientationController(const Eigen::Quaternionf& start_quat);
    OrientationController(const State& state,
                          const State& goal,
                          const Eigen::Vector3f& Kp,
                          const Eigen::Vector3f& Kd,
                          const Eigen::Matrix3f& inertia_matrix);  //@TODO: Should add max torque/w

    void set_state(const Eigen::Quaternionf& q, const Eigen::Vector3f& w);
    void set_state_quaternion(const Eigen::Quaternionf& q);
    void set_goal(const Eigen::Quaternionf& q, const Eigen::Vector3f& w);
    void set_goal_quaternion(const Eigen::Quaternionf& q);
    void update_goal_quaternion(const Eigen::Quaternionf& relative_q);

    State get_state() const;
    Eigen::Quaternionf get_state_quaternion() const;
    State get_goal() const;
    Eigen::Quaternionf get_goal_quaternion() const;

    void update(const float dt);

  private:
    State state;                       // State of controlled system
    State goal;                        // Desired state to which control will drive the system
    Eigen::Vector3f Kp;                // Proportional controller gains
    Eigen::Vector3f Kd;                // Derivative controller gains
    Eigen::Matrix3f inertia_matrix;    // Moment of Inertia, expressed in body frame
    Eigen::Vector3f angular_momentum;  // Expressed in world frame
};
}  // namespace control
