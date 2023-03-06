#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace geometry
{

class CubicBezierCurve
{
  public:
    static constexpr int NUM_SAMPLES = 16;

    CubicBezierCurve(const Eigen::Vector3f& c0,
                     const Eigen::Vector3f& c1,
                     const Eigen::Vector3f& c2,
                     const Eigen::Vector3f& c3);

    Eigen::Vector3f at(const float t) const;

    Eigen::Vector3f project_to_closest_point(const Eigen::Vector3f& pos) const;
    const Eigen::Matrix<float, NUM_SAMPLES, 3>& get_sample_points() const;

    Eigen::Matrix<float, 4, 3> C;

  private:
    Eigen::Matrix<float, NUM_SAMPLES, 3> sample_points;
};
}  // namespace geometry
