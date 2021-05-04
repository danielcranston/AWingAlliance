#include "geometry.h"

namespace geometry
{
// https://stackoverflow.com/questions/14971712/eigen-perspective-projection-matrix
Eigen::Matrix4f
perspective(const float fov_y, const float aspect, const float z_near, const float z_far)
{
    float y_scale = 1.0f / std::tan(fov_y / 2);
    float x_scale = y_scale / aspect;

    Eigen::Matrix4f res;
    res << x_scale, 0, 0, 0, 0, y_scale, 0, 0, 0, 0, -(z_far + z_near) / (z_far - z_near), -1, 0, 0,
        -2 * z_near * z_far / (z_far - z_near), 0;

    return res.transpose();
}

Eigen::Vector3f get_right_dir(const Eigen::Matrix4f& matrix)
{
    // x: first col
    return matrix.col(0).head<3>();
}

Eigen::Vector3f get_up_dir(const Eigen::Matrix4f& matrix)
{
    // y: second col
    return matrix.col(1).head<3>();
}

Eigen::Vector3f get_fwd_dir(const Eigen::Matrix4f& matrix)
{
    // -z: third col (negated)
    return -matrix.col(2).head<3>();
}

Eigen::Matrix4f to_scale_matrix(const Eigen::Vector3f& scale)
{
    // Funny how this is the most direct way I've found so far...
    return scale.homogeneous().asDiagonal().toDenseMatrix();
}
}  // namespace geometry
