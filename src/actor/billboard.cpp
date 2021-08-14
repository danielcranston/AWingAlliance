#include <iostream>

#include "actor/billboard.h"

namespace actor
{
Billboard::Billboard(const std::string& name,
                     const Eigen::Vector3f& position,
                     const Eigen::Quaternionf& orientation,
                     const Eigen::Vector3f& velocity,
                     const Eigen::Vector2f& scale_vec,
                     const float start_time,
                     const float end_time)
  : Actor(name, position, orientation, "quad", ""),
    alive(true),
    start_time(start_time),
    end_time(end_time)
{
    motion_state.velocity = velocity;
    scale = Eigen::Matrix4f::Identity();
    scale.diagonal() = Eigen::Vector3f(scale_vec.x(), scale_vec.y(), 1.0f).homogeneous();
}

bool Billboard::is_alive() const
{
    return alive;
}

float Billboard::get_start_time() const
{
    return start_time;
}

void Billboard::tick(float current_time_s, float dt)
{
    if (alive && current_time_s > end_time)
    {
        alive = false;
    }
    else
    {
        motion_state.position += motion_state.velocity * dt;
    }
}

const Eigen::Matrix4f& Billboard::get_scale() const
{
    return scale;
}
}  // namespace actor
