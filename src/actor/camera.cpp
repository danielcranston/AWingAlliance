#include "actor/camera.h"

namespace actor
{
Camera::Camera(const std::string& name,
               const Eigen::Vector3f& position,
               const Eigen::Quaternionf& orientation,
               const Eigen::Matrix4f& perspective,
               std::function<Eigen::Isometry3f()> tick_behavior_fn,
               const std::string& visual,
               const std::string& geometry)
  : Actor(name, position, orientation, visual, geometry),
    perspective(perspective),
    tick_behavior_fn(tick_behavior_fn)
{
}

void Camera::set_tick_behavior(std::function<Eigen::Isometry3f()> fn)
{
    tick_behavior_fn = fn;
}

void Camera::set_perspective(Eigen::Matrix4f p)
{
    perspective = std::move(p);
}
const Eigen::Matrix4f& Camera::get_perspective()
{
    return perspective;
};

void Camera::tick(float current_time_s, float dt)
{
    auto target_pose = tick_behavior_fn();

    // TODO: Use LQR control to guide camera pose to target pose
    pose = target_pose;
}

}  // namespace actor
