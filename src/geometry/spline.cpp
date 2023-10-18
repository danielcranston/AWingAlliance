#include <iostream>
#include "geometry/spline.h"

namespace
{
// Characteristic matrix
static const Eigen::Matrix4f M = []() {
    auto M = Eigen::Matrix4f();
    M.row(0) << -1, 3, -3, 1;
    M.row(1) << 3, -6, 3, 0;
    M.row(2) << -3, 3, 0, 0;
    M.row(3) << 1, 0, 0, 0;
    return M;
}();
}  // namespace

namespace geometry
{
CubicBezierCurve::CubicBezierCurve(const Eigen::Vector3f& c0,
                                   const Eigen::Vector3f& c1,
                                   const Eigen::Vector3f& c2,
                                   const Eigen::Vector3f& c3)
  : C([&]() {
        Eigen::Matrix<float, 4, 3> C;
        C.row(0) = c0;
        C.row(1) = c1;
        C.row(2) = c2;
        C.row(3) = c3;
        return C;
    }())
{
    for (int i = 0; i < sample_points.rows(); ++i)
    {
        sample_points.row(i) = at(i / static_cast<float>(sample_points.rows()));
    }
}

Eigen::Vector3f CubicBezierCurve::at(const float t) const
{
    auto T = Eigen::Vector4f(std::pow(t, 3), std::pow(t, 2), t, 1);

    return T.transpose() * M * C;
}

Eigen::Vector3f CubicBezierCurve::project_to_closest_point(const Eigen::Vector3f& pos) const
{
    Eigen::Matrix<float, NUM_SAMPLES, 1> distances =
        (sample_points.rowwise() - pos.transpose()).rowwise().squaredNorm();

    Eigen::Index closest_index;
    distances.minCoeff(&closest_index);

    return sample_points.row(closest_index);
}

const Eigen::Matrix<float, CubicBezierCurve::NUM_SAMPLES, 3>&
CubicBezierCurve::get_sample_points() const
{
    return sample_points;
}
}  // namespace geometry
