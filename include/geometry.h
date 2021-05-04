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
}  // namespace geometry
