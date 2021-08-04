#include "geometry/collision.h"

#include <vector>
#include <algorithm>
#include <iostream>

namespace geometry
{
namespace
{
Eigen::Matrix3Xf make_separating_axes(const Eigen::Isometry3f& relative_pose)
{
    Eigen::Matrix3Xf separating_axes(3, 15);

    separating_axes.col(0) = Eigen::Vector3f::UnitX();
    separating_axes.col(1) = Eigen::Vector3f::UnitY();
    separating_axes.col(2) = Eigen::Vector3f::UnitZ();
    separating_axes.col(3) = relative_pose.linear() * Eigen::Vector3f::UnitX();
    separating_axes.col(4) = relative_pose.linear() * Eigen::Vector3f::UnitY();
    separating_axes.col(5) = relative_pose.linear() * Eigen::Vector3f::UnitZ();

    // It's actually the cross product between the EDGES, but for cubes normals == edges
    // https://www.gamedev.net/forums/topic/694911-separating-axis-theorem-3d-polygons/5420814/
    for (int i = 0; i < 3; ++i)
    {
        separating_axes.col(6 + (3 * i)) = separating_axes.col(i).cross(separating_axes.col(3));
        separating_axes.col(7 + (3 * i)) = separating_axes.col(i).cross(separating_axes.col(4));
        separating_axes.col(8 + (3 * i)) = separating_axes.col(i).cross(separating_axes.col(5));
    }

    return separating_axes;
};
}  // namespace

CollisionBox::CollisionBox(const Eigen::Vector3f& extents)
  : extents(extents), corners([&extents]() {
        Eigen::Matrix3Xf ret(3, 8);
        ret.col(0) << -0.5f, -0.5f, -0.5f;
        ret.col(1) << 0.5f, -0.5f, -0.5f;
        ret.col(2) << 0.5f, 0.5f, -0.5f;
        ret.col(3) << -0.5f, 0.5f, -0.5f;
        ret.col(4) << -0.5f, -0.5f, 0.5f;
        ret.col(5) << 0.5f, -0.5f, 0.5f;
        ret.col(6) << 0.5f, 0.5f, 0.5f;
        ret.col(7) << -0.5f, 0.5f, 0.5f;

        for (int i = 0; i < ret.cols(); ++i)
        {
            ret.col(i) = ret.col(i).cwiseProduct(extents);
        }

        return ret;
    }()){};

bool CollisionBox::is_inside(const CollisionBox& other,
                             const Eigen::Isometry3f& relative_pose) const
{
    auto separating_axes = make_separating_axes(relative_pose);

    Eigen::Matrix3Xf transformed_corners(3, 8);
    for (int i = 0; i < other.corners.cols(); ++i)
    {
        transformed_corners.col(i) = relative_pose * other.corners.col(i);
    }

    for (int i = 0; i < separating_axes.cols(); ++i)
    {
        if (is_separating_axis(separating_axes.col(i), corners, transformed_corners))
        {
            return false;
        }
    }

    return true;
}

bool CollisionBox::is_inside(const Eigen::Matrix3Xf& points,
                             const Eigen::Isometry3f& relative_pose) const
{
    for (int i = 0; i < points.cols(); ++i)
    {
        const Eigen::Vector3f p = relative_pose * points.col(i);

        if ((p.x() < extents.x() / 2.0f && p.x() > -extents.x() / 2.0f) &&
            (p.y() < extents.y() / 2.0f && p.y() > -extents.y() / 2.0f) &&
            (p.z() < extents.z() / 2.0f && p.z() > -extents.z() / 2.0f))
        {
            return true;
        }
    }
    return false;
}

bool intersects(const Eigen::Vector3f& orig,
                const Eigen::Vector3f& dir,
                CollisionBox aabb,
                float tmin,
                float tmax)
{
    // https://medium.com/@bromanz/another-view-on-the-classic-ray-aabb-intersection-algorithm-for-bvh-traversal-41125138b525
    Eigen::Vector3f invD = Eigen::Vector3f::Ones().cwiseQuotient(dir);
    Eigen::Vector3f t0s = (-aabb.extents / 2.0f - orig).cwiseProduct(invD);
    Eigen::Vector3f t1s = (aabb.extents / 2.0f - orig).cwiseProduct(invD);

    auto tsmaller = t0s.cwiseMin(t1s);
    auto tbigger = t0s.cwiseMax(t1s);

    tmin = std::max(tmin, std::max(tsmaller.x(), std::max(tsmaller.y(), tsmaller.z())));
    tmax = std::min(tmax, std::min(tbigger.x(), std::min(tbigger.y(), tbigger.z())));

    return (tmin < tmax);
}

bool is_separating_axis(const Eigen::Vector3f& axis,
                        const Eigen::Matrix3Xf& pointsA,
                        const Eigen::Matrix3Xf& pointsB)
{
    // https://en.wikipedia.org/wiki/Hyperplane_separation_theorem#Use_in_collision_detection

    // Project the points from the two objects onto the axis
    auto projectionsA = (axis.transpose() * pointsA).transpose();
    auto projectionsB = (axis.transpose() * pointsB).transpose();

    // Axis is separating if the projections of object B and object A onto the axis have no overlap
    if (projectionsB.minCoeff() > projectionsA.maxCoeff() ||
        projectionsB.maxCoeff() < projectionsA.minCoeff())
    {
        return true;
    }

    return false;
}
}  // namespace geometry