#pragma once
#include <iostream>

#include <Eigen/Geometry>

namespace control
{
// float smoothstep(float edge0, float edge1, float x)
// {
//     // Scale, bias and saturate x to 0..1 range
//     x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
//     // Evaluate polynomial
//     return x * x * (3 - 2 * x);
// }

class MotionModel
{
    // https://gafferongames.com/post/integration_basics/
    // https://gafferongames.com/post/physics_in_3d/
  public:
    MotionModel();

    Eigen::Isometry3f update(const float d_v,
                             const Eigen::Vector3f& d_w,
                             const Eigen::Vector3f& position,
                             const Eigen::Quaternionf& orientation,
                             const float dt);

  private:
    Eigen::Vector3f attitude_inputs;  // between [-1, 1]
    Eigen::Vector3f attitude_windup_times;

    Eigen::Vector3f max_angular_velocity;

    Eigen::Matrix<float, 3, 2> attitude_input_bounds;
    Eigen::Matrix<float, 3, 2> attitude_output_bounds;

    Eigen::Vector2f throttle_input_bounds;
    Eigen::Vector2f throttle_output_bounds;

    float throttle_input;  // Between [-1, 1]
    float throttle_windup_time;
    float max_speed = 50.0f;

    float input_falloff_rate = 0.9f;
};
}  // namespace control
