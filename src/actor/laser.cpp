#include <iostream>

#include "actor/laser.h"

namespace actor
{
Laser::Laser(const std::string& name,
             const Eigen::Vector3f& position,
             const Eigen::Quaternionf& orientation,
             const Eigen::Vector3f& scale_vec,
             const Eigen::Vector3f& color,
             const float speed,
             const std::string& visual,
             const std::string& geometry)
  : Actor(name, position, orientation, visual, geometry), alive(true), color(color), speed(speed)
{
    scale = Eigen::Matrix4f::Identity();
    scale.diagonal() = scale_vec.homogeneous();
}

void Laser::tick(float current_time_s, float dt)
{
    motion_state.position += get_fwd_dir() * speed * dt;
}

const Eigen::Vector3f& Laser::get_color()
{
    return color;
}
const Eigen::Matrix4f& Laser::get_scale()
{
    return scale;
}

}  // namespace actor