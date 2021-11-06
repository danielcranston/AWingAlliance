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

    for (int i = 0; i < a.num_face_normals(); ++i)
    {
        separating_axes.col(i) = a.face_normals.col(i);
    }

    auto start_idx = a.num_face_normals();
    for (int i = 0; i < b.num_face_normals(); ++i)
    {
        separating_axes.col(start_idx + i) = face_normals.col(i);
    }

    start_idx = a.num_face_normals() + b.num_face_normals();
    int idx = 0;
    for (int i = 0; i < a.num_edges(); ++i)
    {
        for (int j = 0; j < b.num_edges(); ++j)
        {
            separating_axes.col(start_idx + idx++) = a.edges.col(i).cross(edges.col(j));
        }
    }

    return separating_axes;
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
        for (int i = 0; i < data.vertices.size() / 3; ++i)
        {
            vertices.col(i) = Eigen::Vector3f(
                data.vertices[3 * i + 0], data.vertices[3 * i + 1], data.vertices[3 * i + 2]);
        }

        face_normals.resize(3, data.face_indices.size());
        for (int i = 0; i < data.face_indices.size(); ++i)
        {
            const auto dir1 = vertices.col(std::get<0>(data.face_indices[i])) -
                              vertices.col(std::get<1>(data.face_indices[i]));
            const auto dir2 = vertices.col(std::get<0>(data.face_indices[i])) -
                              vertices.col(std::get<2>(data.face_indices[i]));
            face_normals.col(i) = dir1.cross(dir2).normalized();
        }

        edges.resize(3, data.edge_indices.size());

        int i = 0;
        for (const auto& [idx1, idx2] : data.edge_indices)
        {
            edges.col(i++) = (vertices.col(idx1) - vertices.col(idx2)).normalized();
        }

        min = Eigen::Vector3f(data.min.value()[0], data.min.value()[1], data.min.value()[2]);
        max = Eigen::Vector3f(data.max.value()[0], data.max.value()[1], data.max.value()[2]);
    }
}

bool intersects(const CollisionShape& a,
                const CollisionShape& b,
                const Eigen::Isometry3f& relative_pose)
{
    auto separating_axes =
        make_separating_axes(a, b, Eigen::Isometry3f(Eigen::Quaternionf(relative_pose.linear())));

    auto vertices = relative_pose * b.vertices.colwise().homogeneous();

    for (int i = 0; i < separating_axes.cols(); ++i)
    {
        if (is_separating_axis(separating_axes.col(i), a.vertices, vertices))
        {
            return false;
        }
    }

    return true;
}

bool is_inside(const CollisionShape& box,
               const Eigen::Matrix3Xf& points,
               const Eigen::Isometry3f& relative_pose)

{
    auto transformed_points = relative_pose * points.colwise().homogeneous();

    for (int i = 0; i < box.num_edges(); ++i)
    {
        if (is_separating_axis(box.edges.col(i), box.vertices, points))
        {
            return false;
        }
    }
    for (int i = 0; i < box.num_face_normals(); ++i)
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

    // https://medium.com/@bromanz/another-view-on-the-classic-ray-aabb-intersection-algorithm-for-bvh-traversal-41125138b525
    Eigen::Vector3f invD = Eigen::Vector3f::Ones().cwiseQuotient(transformed_ray.direction);
    Eigen::Vector3f t0s = (-extents / 2.0f - transformed_ray.origin).cwiseProduct(invD);
    Eigen::Vector3f t1s = (extents / 2.0f - transformed_ray.origin).cwiseProduct(invD);

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