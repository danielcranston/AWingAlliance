#include "geometry/collision.h"

#include <vector>
#include <algorithm>
#include <iostream>

namespace geometry
{
namespace
{
std::tuple<Eigen::Matrix3Xf, Eigen::Matrix3Xf, Eigen::Matrix3Xf>
make_box_attribs(const Eigen::Vector3f& min, const Eigen::Vector3f& max)
{
    const auto mean = (max + min) / 2.0f;
    const auto scale = max - min;

    auto vertices = Eigen::Matrix3Xf(3, 8);
    vertices.col(0) << -0.5f, -0.5f, -0.5f;
    vertices.col(1) << 0.5f, -0.5f, -0.5f;
    vertices.col(2) << 0.5f, 0.5f, -0.5f;
    vertices.col(3) << -0.5f, 0.5f, -0.5f;
    vertices.col(4) << -0.5f, -0.5f, 0.5f;
    vertices.col(5) << 0.5f, -0.5f, 0.5f;
    vertices.col(6) << 0.5f, 0.5f, 0.5f;
    vertices.col(7) << -0.5f, 0.5f, 0.5f;

    // Surely you can do better with Eigen ...
    for (int i = 0; i < vertices.cols(); ++i)
    {
        vertices.col(i) = vertices.col(i).cwiseProduct(scale) + mean;
    }

    auto face_normals = Eigen::Matrix3Xf(3, 3);
    face_normals.col(0) = Eigen::Vector3f::UnitX();
    face_normals.col(1) = Eigen::Vector3f::UnitY();
    face_normals.col(2) = Eigen::Vector3f::UnitZ();

    auto edges = face_normals;

    return std::make_tuple(vertices, face_normals, edges);
}

Eigen::Matrix3Xf make_separating_axes(const CollisionShape& a,
                                      const CollisionShape& b,
                                      const Eigen::Isometry3f& relative_pose)
{
    // Bring shape B into A's frame first
    auto face_normals = relative_pose * b.face_normals.colwise().homogeneous();
    auto edges = relative_pose * b.edges.colwise().homogeneous();

    auto num_cols = a.num_face_normals() + b.num_face_normals() + (a.num_edges() * b.num_edges());
    Eigen::Matrix3Xf separating_axes(3, num_cols);

    for (std::size_t i = 0; i < a.num_face_normals(); ++i)
    {
        separating_axes.col(i) = a.face_normals.col(i);
    }

    auto start_idx = a.num_face_normals();
    for (std::size_t i = 0; i < b.num_face_normals(); ++i)
    {
        separating_axes.col(start_idx + i) = face_normals.col(i);
    }

    start_idx = a.num_face_normals() + b.num_face_normals();
    int idx = 0;
    for (std::size_t i = 0; i < a.num_edges(); ++i)
    {
        for (std::size_t j = 0; j < b.num_edges(); ++j)
        {
            separating_axes.col(start_idx + idx++) = a.edges.col(i).cross(edges.col(j));
        }
    }

    return separating_axes;
}

bool convex_intersection_test(const Eigen::Matrix3Xf& separating_axes,
                              const Eigen::Matrix3Xf& points1,
                              const Eigen::Matrix3Xf& points2)
{
    for (int i = 0; i < separating_axes.cols(); ++i)
    {
        if (is_separating_axis(separating_axes.col(i), points1, points2))
        {
            return false;
        }
    }
    return true;
}

std::pair<Eigen::Vector3f, Eigen::Vector3f> calculate_extents(const CollisionShape& shape)
{
    if (shape.children.empty())
    {
        return std::make_pair(shape.min, shape.max);
    }
    else
    {
        Eigen::Vector3f min = Eigen::Vector3f(std::numeric_limits<float>::max(),
                                              std::numeric_limits<float>::max(),
                                              std::numeric_limits<float>::max());
        Eigen::Vector3f max = Eigen::Vector3f(std::numeric_limits<float>::lowest(),
                                              std::numeric_limits<float>::lowest(),
                                              std::numeric_limits<float>::lowest());

        for (const auto& child : shape.children)
        {
            auto [new_min, new_max] = calculate_extents(child);

            max = max.cwiseMax(new_max);
            min = min.cwiseMin(new_min);
        }

        return std::make_pair(min, max);
    }
}
}  // namespace

CollisionShape::CollisionShape(const Eigen::Vector3f& min, const Eigen::Vector3f& max)
  : min(min), max(max)
{
    // If given ony extents, assume Box
    std::tie(vertices, face_normals, edges) = make_box_attribs(min, max);
}

CollisionShape::CollisionShape(const resources::GeometryData& data)
{
    if (data.children.size())
    {
        std::transform(data.children.begin(),
                       data.children.end(),
                       std::back_inserter(children),
                       [](const auto& child_node) { return CollisionShape(child_node.second); });

        // Calculate combined extents...
        std::tie(min, max) = calculate_extents(*this);
        std::tie(vertices, face_normals, edges) = make_box_attribs(min, max);
    }
    else
    {
        if (data.vertices.empty() || data.face_indices.empty() || data.edge_indices.empty())
        {
            throw std::runtime_error("GeometryData Leaf node has no vertices and/or face/edge "
                                     "indices!");
        }

        if (!(data.min.has_value() && data.max.has_value()))
        {
            throw std::runtime_error("GeometryData Leaf node has no extents!");
        }

        vertices.resize(3, data.vertices.size() / 3);
        for (std::size_t i = 0; i < data.vertices.size() / 3; ++i)
        {
            vertices.col(i) = Eigen::Vector3f(
                data.vertices[3 * i + 0], data.vertices[3 * i + 1], data.vertices[3 * i + 2]);
        }

        face_normals.resize(3, data.face_indices.size());
        for (std::size_t i = 0; i < data.face_indices.size(); ++i)
        {
            const auto dir1 = vertices.col(std::get<0>(data.face_indices[i])) -
                              vertices.col(std::get<1>(data.face_indices[i]));
            const auto dir2 = vertices.col(std::get<0>(data.face_indices[i])) -
                              vertices.col(std::get<2>(data.face_indices[i]));
            face_normals.col(i) = dir1.cross(dir2).normalized();
        }

        edges.resize(3, data.edge_indices.size());

        std::size_t i = 0;
        for (const auto& [idx1, idx2] : data.edge_indices)
        {
            edges.col(i++) = (vertices.col(idx1) - vertices.col(idx2)).normalized();
        }

        min = Eigen::Vector3f(data.min.value()[0], data.min.value()[1], data.min.value()[2]);
        max = Eigen::Vector3f(data.max.value()[0], data.max.value()[1], data.max.value()[2]);
    }
}

std::optional<CollisionShape> intersect_test(const CollisionShape& a,
                                             const CollisionShape& b,
                                             const Eigen::Isometry3f& relative_pose)
{
    if (convex_intersection_test(
            make_separating_axes(a, b, Eigen::Isometry3f(relative_pose.linear())),
            a.vertices,
            relative_pose * b.vertices.colwise().homogeneous()))
    {
        // If B is a leaf, A and B intersect
        if (!b.children.size())
        {
            return b;
        }

        // If B has children, run down the tree
        for (const auto& b_child : b.children)
        {
            if (const auto& test = intersect_test(a, b_child, relative_pose))
            {
                return b_child;
            }
        }
    }

    return std::nullopt;
}

bool is_inside(const CollisionShape& box, const Eigen::Matrix3Xf& points)

{
    for (std::size_t i = 0; i < box.num_edges(); ++i)
    {
        if (is_separating_axis(box.edges.col(i), box.vertices, points))
        {
            return false;
        }
    }
    for (std::size_t i = 0; i < box.num_face_normals(); ++i)
    {
        if (is_separating_axis(box.face_normals.col(i), box.vertices, points))
        {
            return false;
        }
    }
    return true;
}

// http://adrianboeing.blogspot.com/2010/02/intersection-of-convex-hull-with-line.html
// https://www.cubic.org/docs/hermite.htm

bool intersects(const Ray& ray,
                const Eigen::Vector3f& extents,
                const Eigen::Isometry3f& relative_pose,
                float tmin,
                float tmax)
{
    // Bring ray into the AABB frame
    const auto& transformed_ray = ray * relative_pose;

    return ray_aabb_test(transformed_ray.direction, transformed_ray.origin, tmax, tmin, extents);
}

bool ray_aabb_test(const Eigen::Vector3f& ray_dir,
                   const Eigen::Vector3f& ray_origin,
                   float tmax,
                   float tmin,
                   const Eigen::Vector3f& extents)
{
    // https://medium.com/@bromanz/another-view-on-the-classic-ray-aabb-intersection-algorithm-for-bvh-traversal-41125138b525
    Eigen::Vector3f invD = Eigen::Vector3f::Ones().cwiseQuotient(ray_dir);
    Eigen::Vector3f t0s = (-extents / 2.0f - ray_origin).cwiseProduct(invD);
    Eigen::Vector3f t1s = (extents / 2.0f - ray_origin).cwiseProduct(invD);

    auto tsmaller = t0s.cwiseMin(t1s);
    auto tbigger = t0s.cwiseMax(t1s);

    tmin = std::max(tmin, std::max(tsmaller.x(), std::max(tsmaller.y(), tsmaller.z())));
    tmax = std::min(tmax, std::min(tbigger.x(), std::min(tbigger.y(), tbigger.z())));

    return (tmin < tmax);
}

bool ray_aabb_test(const Eigen::Isometry3f& T_world_ray,
                   const float ray_tmax,
                   const float ray_tmin,
                   const Eigen::Isometry3f& T_world_aabb,
                   const Eigen::Vector3f& aabb_dimensions)
{
    // Bring ray into AABB frame
    const auto T_aabb_ray = T_world_aabb.inverse() * T_world_ray;
    const auto ray_dir = T_aabb_ray.linear().col(0);
    const auto ray_pos = T_aabb_ray.translation();

    return ray_aabb_test(ray_dir, ray_pos, ray_tmax, ray_tmin, aabb_dimensions);
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
