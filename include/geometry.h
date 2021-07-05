#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace geometry
{
struct MotionState
{
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Eigen::Vector3f position = Eigen::Vector3f::Zero();
    Eigen::Quaternionf orientation = Eigen::Quaternionf::Identity();

    // Expressed in the world frame (as opposed to body/local frame)
    Eigen::Vector3f velocity = Eigen::Vector3f::Zero();
    Eigen::Vector3f acceleration = Eigen::Vector3f::Zero();

    Eigen::Vector3f angular_velocity = Eigen::Vector3f::Zero();
    // Eigen::Vector3f angular_acceleration = Eigen::Vector3f::Zero();
    Eigen::Vector3f angular_momentum = Eigen::Vector3f::Zero();

    Eigen::Matrix3f inertia_matrix = Eigen::Matrix3f::Identity();
    // Note: Inertial center of mass is always assumed to be identity.
};

// https://stackoverflow.com/questions/14971712/eigen-perspective-projection-matrix
Eigen::Matrix4f
perspective(const float fov_y, const float aspect, const float z_near, const float z_far);

Eigen::Vector3f get_right_dir(const Eigen::Matrix3f& matrix);
Eigen::Vector3f get_up_dir(const Eigen::Matrix3f& matrix);
Eigen::Vector3f get_fwd_dir(const Eigen::Matrix3f& matrix);

Eigen::Matrix4f to_scale_matrix(const Eigen::Vector3f& scale);

Eigen::Isometry3f make_pose(const Eigen::Vector3f& pos, const Eigen::Quaternionf& quat);
Eigen::Isometry3f make_pose(const Eigen::Vector3f& pos);

Eigen::Vector3f lerp(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2, const float t);

Eigen::Quaternionf slerp(const Eigen::Quaternionf& q1, const Eigen::Quaternionf& q2, const float t);

Eigen::Quaternionf look_at(const Eigen::Vector3f& from,
                           const Eigen::Vector3f& to,
                           const Eigen::Vector3f& right);

Eigen::Quaternionf angular_velocity_to_quat(const Eigen::Vector3f& w, const float dt);

float map(const Eigen::Vector2f& in_bounds, const Eigen::Vector2f& out_bounds, float x);

Eigen::Vector3f map_vector(const Eigen::Matrix<float, 3, 2>& input_bounds,
                           const Eigen::Matrix<float, 3, 2>& output_bounds,
                           const Eigen::Vector3f v);

Eigen::Vector3f clamp(const Eigen::Vector3f& v,
                      const Eigen::Vector3f& lb,
                      const Eigen::Vector3f& ub);

}  // namespace geometry
