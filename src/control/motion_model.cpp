#include "geometry.h"

#include "control/motion_model.h"

namespace control
{
Eigen::Isometry3f MotionModel::update(const MotionControl::StateMap& state_map,
                                      const Eigen::Vector3f& position,
                                      const Eigen::Quaternionf& orientation,
                                      const float t,
                                      const float dt)
{
    float d_yaw = 0.0f;
    float d_pitch = 0.0f;
    float d_roll = 0.0f;
    float d_speed = 0.0f;

    float scale = 3.0f * dt;

    if (state_map.test(static_cast<int>(MotionControl::States::TURN_LEFT)))
    {
        d_yaw += scale;
        d_roll -= 0.25 * scale;
    }
    if (state_map.test(static_cast<int>(MotionControl::States::TURN_RIGHT)))
    {
        d_yaw -= scale;
        d_roll += 0.25 * scale;
    }
    if (state_map.test(static_cast<int>(MotionControl::States::TURN_UP)))
    {
        d_pitch -= scale;
    }
    if (state_map.test(static_cast<int>(MotionControl::States::TURN_DOWN)))
    {
        d_pitch += scale;
    }
    if (state_map.test(static_cast<int>(MotionControl::States::ROLL_LEFT)))
    {
        d_roll -= scale;
    }
    if (state_map.test(static_cast<int>(MotionControl::States::ROLL_RIGHT)))
    {
        d_roll += scale;
    }
    if (state_map.test(static_cast<int>(MotionControl::States::ACC_INCREASE)))
    {
        d_speed += scale;
    }
    if (state_map.test(static_cast<int>(MotionControl::States::ACC_DECREASE)))
    {
        d_speed -= scale;
    }

    if (d_pitch == 0.0f || (d_pitch < 0.0f && pitch > 0.0f) || (d_pitch > 0.0f && pitch < 0.0f))
    {
        pitch *= (1.0f - 3.0f * dt);
    }
    if (d_yaw == 0.0f || (d_yaw < 0.0f && yaw > 0.0f) || (d_yaw > 0.0f && yaw < 0.0f))
    {
        yaw *= (1.0f - 3.0f * dt);
    }
    if (d_roll == 0.0f || (d_roll < 0.0f && roll > 0.0f) || (d_roll > 0.0f && roll < 0.0f))
    {
        roll *= (1.0f - 3.0f * dt);
    }
    if (d_roll == 0.0f || (d_roll < 0.0f && roll > 0.0f) || (d_roll > 0.0f && roll < 0.0f))
    {
        roll *= (1.0f - 3.0f * dt);
    }
    if (d_speed == 0.0f || (d_speed < 0.0f && speed > 0.0f) || (d_speed > 0.0f && speed < 0.0f))
    {
        speed *= 0.99f;
    }

    yaw = std::clamp(yaw + d_yaw, -1.0f, 1.0f);
    pitch = std::clamp(pitch + d_pitch, -1.0f, 1.0f);
    roll = std::clamp(roll + d_roll, -1.0f, 1.0f);
    speed = std::clamp(speed + d_speed, -0.25f, 1.0f);

    // std::cout << "yaw, pitch: " << yaw << " " << pitch << std::endl;
    auto q = Eigen::AngleAxis<float>(max_yaw_speed * yaw * dt, Eigen::Vector3f::UnitY()) *
             Eigen::AngleAxis<float>(max_pitch_speed * pitch * dt, Eigen::Vector3f::UnitX()) *
             Eigen::AngleAxis<float>(max_roll_speed * roll * -dt, Eigen::Vector3f::UnitZ());

    auto ret = Eigen::Isometry3f((orientation * q).normalized());
    ret.translation() = position - ret.linear().col(2) * max_speed * speed * dt;
    return ret;
}
}  // namespace control
