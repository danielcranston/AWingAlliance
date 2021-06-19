#include "geometry.h"

#include <iostream>

#include "control/motion_model.h"

namespace control
{
MotionModel::MotionModel()
  : attitude_inputs(Eigen::Vector3f::Zero()),
    attitude_windup_times({ 0.25f, 0.25f, 0.25f }),
    max_angular_velocity({ M_PI / 8.0f, M_PI / 24.0f, M_PI / 8.0f })
{
    throttle_windup_time = 0.5f;
    attitude_input_bounds << -Eigen::Vector3f::Ones(), Eigen::Vector3f::Ones();
    attitude_output_bounds << -max_angular_velocity, max_angular_velocity;
    throttle_input_bounds << -1.0f, 1.0f;
    throttle_output_bounds << 0.0f, 1.0f;
}

Eigen::Isometry3f MotionModel::update(const float d_v,
                                      const Eigen::Vector3f& d_w,
                                      const Eigen::Vector3f& position,
                                      const Eigen::Quaternionf& orientation,
                                      const float t,
                                      const float dt)
{
    // throttle
    throttle_input += dt * d_v / throttle_windup_time;

    throttle_input =
        std::clamp(throttle_input, throttle_input_bounds.x(), throttle_input_bounds.y());

    auto throttle = geometry::map(throttle_input_bounds, throttle_output_bounds, throttle_input);

    // Attitude
    attitude_inputs += dt * (d_w.cwiseQuotient(attitude_windup_times));
    attitude_inputs.x() *= d_w.x() == 0.0f ? input_falloff_rate : 1.0f;
    attitude_inputs.y() *= d_w.y() == 0.0f ? input_falloff_rate : 1.0f;
    attitude_inputs.z() *= d_w.z() == 0.0f ? input_falloff_rate : 1.0f;

    attitude_inputs = geometry::clamp(
        attitude_inputs, attitude_input_bounds.col(0), attitude_input_bounds.col(1));

    auto q = geometry::angular_velocity_to_quat(
        geometry::map_vector(attitude_input_bounds, attitude_output_bounds, attitude_inputs), 1.0);

    auto ret = Eigen::Isometry3f((orientation * q).normalized());
    ret.translation() = position - ret.linear().col(2) * max_speed * throttle;
    return ret;
}
}  // namespace control
