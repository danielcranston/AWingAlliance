#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace geometry
{
struct CollisionBox
{
    explicit CollisionBox(const Eigen::Vector3f& extents);

    bool is_inside(const CollisionBox& other,
                   const Eigen::Isometry3f& relative_pose) const;  // brings extents of other into
                                                                   // this box frame

    bool is_inside(const Eigen::Matrix3Xf& points,                 // In world frame
                   const Eigen::Isometry3f& relative_pose) const;  // brings points into this box
                                                                   // frame

    const Eigen::Vector3f extents;
    const Eigen::Matrix3Xf corners;
};

bool intersects(const Eigen::Vector3f& orig,  // Expressed in collision box frame
                const Eigen::Vector3f& dir,   // Expressed in collision box frame
                CollisionBox aabb,
                float tmin,   // Min projection
                float tmax);  // Max projection

bool is_separating_axis(const Eigen::Vector3f& axis,
                        const Eigen::Matrix3Xf& pointsA,
                        const Eigen::Matrix3Xf& pointsB);
}  // namespace geometry
