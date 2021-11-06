#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "resources/geometry_data.h"

namespace geometry
{
struct Ray
{
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Ray(const Eigen::Vector3f& origin, const Eigen::Vector3f& direction)
      : origin(origin), direction(direction)
    {
    }

    Ray operator*(const Eigen::Isometry3f& transform) const
    {
        return Ray(transform * origin, (transform.linear() * direction).transpose());
    }
    Eigen::Vector3f origin;
    Eigen::Vector3f direction;
};

struct CollisionShape
{
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    explicit CollisionShape(const Eigen::Vector3f& min, const Eigen::Vector3f& max);

    CollisionShape(const resources::GeometryData& data);

    Eigen::Matrix3Xf vertices;
    Eigen::Matrix3Xf face_normals;
    Eigen::Matrix3Xf edges;

    Eigen::Vector3f min;
    Eigen::Vector3f max;

    Eigen::Vector3f scale() const
    {
        return max - min;
    }
    inline std::size_t num_vertices() const
    {
        return vertices.cols();
    }
    inline std::size_t num_face_normals() const
    {
        return face_normals.cols();
    }
    inline std::size_t num_edges() const
    {
        return edges.cols();
    }

    std::vector<CollisionShape> children;
};

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

bool is_inside(const CollisionShape& box,
               const Eigen::Matrix3Xf& points,
               const Eigen::Isometry3f& relative_pose);

bool intersects(const CollisionShape& a,
                const CollisionShape& b,
                const Eigen::Isometry3f& relative_pose);

bool intersects(const Ray& ray,
                const Eigen::Vector3f& extents,          // Extents of AABB
                const Eigen::Isometry3f& relative_pose,  // Brings Ray into AABB frame
                float tmin,                              // Min projection
                float tmax);                             // Max projection

bool is_separating_axis(const Eigen::Vector3f& axis,
                        const Eigen::Matrix3Xf& pointsA,
                        const Eigen::Matrix3Xf& pointsB);
}  // namespace geometry
