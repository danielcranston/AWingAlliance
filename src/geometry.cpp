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

Eigen::Isometry3f make_pose(const Eigen::Vector3f& pos, const Eigen::Quaternionf& quat)
{
    auto pose = Eigen::Isometry3f(quat);
    pose.translation() = pos;
    return pose;
}

Eigen::Isometry3f make_pose(const Eigen::Vector3f& pos)
{
    return make_pose(pos, Eigen::Quaternionf::Identity());
}

Eigen::Vector3f lerp(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2, const float t)
{
    return t * v1 + (1.0f - t) * v2;
}

Eigen::Quaternionf slerp(const Eigen::Quaternionf& q1, const Eigen::Quaternionf& q2, const float t)
{
    return q1.slerp(t, q2);
}

Eigen::Quaternionf look_at(const Eigen::Vector3f& from,
                           const Eigen::Vector3f& to,
                           const Eigen::Vector3f& right)
{
    Eigen::Vector3f new_z = -(to - from).normalized();
    Eigen::Vector3f new_y = new_z.cross(right);
    Eigen::Vector3f new_x = new_y.cross(new_z);

    auto R = Eigen::Matrix3f();
    R << new_x, new_y, new_z;

    return Eigen::Quaternionf(R);
}

Eigen::Quaternionf angular_velocity_to_quat(const Eigen::Vector3f& w, const float dt)
{
    auto angle = w.norm();
    auto axis = w.normalized();
    return Eigen::Quaternionf(Eigen::AngleAxisf(angle * dt, axis));
}
}  // namespace geometry
