#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace geometry
{
// https://stackoverflow.com/questions/14971712/eigen-perspective-projection-matrix
Eigen::Matrix4f
perspective(const float fov_y, const float aspect, const float z_near, const float z_far);

Eigen::Vector3f get_right_dir(const Eigen::Matrix4f& matrix);
Eigen::Vector3f get_up_dir(const Eigen::Matrix4f& matrix);
Eigen::Vector3f get_fwd_dir(const Eigen::Matrix4f& matrix);

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
