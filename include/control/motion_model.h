#pragma once
#include <iostream>

#include <Eigen/Geometry>

#include "control/motion_control.h"
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
    MotionModel() = default;

    Eigen::Isometry3f update(const MotionControl::StateMap& state_map,
                             const Eigen::Vector3f& position,
                             const Eigen::Quaternionf& orientation,
                             const float t,
                             const float dt);

  private:
    Eigen::Vector3f position;
    Eigen::Quaternionf orientation;

    float speed = 0.0f;
    float yaw = 0.0f;
    float roll = 0.0f;
    float pitch = 0.0f;

    float max_speed = 130.0f;
    float max_pitch_speed = 2.0f;
    float max_yaw_speed = 1.0f;
    float max_roll_speed = 2.0f;
};
}  // namespace control