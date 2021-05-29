#include "geometry.h"

#include "control/motion_model.h"

namespace control
{
namespace
{
float test_state(const MotionControl::StateMap& state_map,
                 MotionControl::States state1,
                 MotionControl::States state2)
{
    if (state_map.test(state1))
        return -1.0f;
    if (state_map.test(state2))
        return 1.0f;
    else
        return 0.0f;
}

std::pair<Eigen::Vector3f, float> process_state_map(const MotionControl::StateMap& state_map)
{
    Eigen::Vector3f d_attitude = {
        test_state(state_map, MotionControl::States::TURN_UP, MotionControl::States::TURN_DOWN),
        test_state(state_map, MotionControl::States::TURN_RIGHT, MotionControl::States::TURN_LEFT),
        test_state(state_map, MotionControl::States::ROLL_RIGHT, MotionControl::States::ROLL_LEFT)
    };

    float d_speed = test_state(
        state_map, MotionControl::States::ACC_DECREASE, MotionControl::States::ACC_INCREASE);

    return { d_attitude, d_speed };
}
}  // namespace

MotionModel::MotionModel()
  : attitude_inputs(Eigen::Vector3f::Zero()),
    attitude_windup_times({ 0.25f, 0.25f, 0.25f }),
    max_angular_velocity({ M_PI / 4.0f, M_PI / 4.0f, M_PI / 4.0f })
{
    attitude_input_bounds << -Eigen::Vector3f::Ones(), Eigen::Vector3f::Ones();
    attitude_output_bounds << -max_angular_velocity, max_angular_velocity;
    speed_input_bounds << -0.25f, 1.0f;
}

Eigen::Isometry3f MotionModel::update(const MotionControl::StateMap& state_map,
                                      const Eigen::Vector3f& position,
                                      const Eigen::Quaternionf& orientation,
                                      const float t,
                                      const float dt)
{
    auto [d_attitude, d_speed] = process_state_map(state_map);

    d_speed *= dt / speed_windup_time;

    attitude_inputs += dt * (d_attitude.cwiseQuotient(attitude_windup_times));
    attitude_inputs.x() *= d_attitude.x() == 0.0f ? input_falloff_rate : 1.0f;
    attitude_inputs.y() *= d_attitude.y() == 0.0f ? input_falloff_rate : 1.0f;
    attitude_inputs.z() *= d_attitude.z() == 0.0f ? input_falloff_rate : 1.0f;

    attitude_inputs = geometry::clamp(
        attitude_inputs, attitude_input_bounds.col(0), attitude_input_bounds.col(1));

    if (d_speed == 0.0f)
        speed *= input_falloff_rate;
    else
        speed = std::clamp(speed + d_speed, speed_input_bounds.x(), speed_input_bounds.y());

    auto q = geometry::angular_velocity_to_quat(
        geometry::map_vector(attitude_input_bounds, attitude_output_bounds, attitude_inputs), 1.0);

    auto ret = Eigen::Isometry3f((orientation * q).normalized());
    ret.translation() = position - ret.linear().col(2) * max_speed * speed * dt;
    return ret;
}
}  // namespace control
