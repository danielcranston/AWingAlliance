#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace geometry
{
struct CollisionBox
{
    CollisionBox(const Eigen::Vector3f& extents);

    bool is_inside(const CollisionBox& other,
                   const Eigen::Isometry3f& relative_pose) const;  // brings extents of other into
                                                                   // this box frame

    const Eigen::Vector3f extents;
    const Eigen::Matrix3Xf corners;
};

bool is_separating_axis(const Eigen::Vector3f& axis,
                        const Eigen::Matrix3Xf& pointsA,
                        const Eigen::Matrix3Xf& pointsB);
}  // namespace geometry
