#pragma once

#include "actor/actor.h"

#include "control/camera_controller.h"

namespace actor
{
class Camera : public Actor
{
  public:
    Camera(const std::string& name,
           const Eigen::Vector3f& position,
           const Eigen::Quaternionf& orientation,
           const Eigen::Matrix4f& perspective,
           std::function<Eigen::Isometry3f()> tick_behavior_fn =
               []() { return Eigen::Isometry3f::Identity(); },
           const std::string& visual = "",
           const std::string& geometry = "");

    void set_tick_behavior(std::function<Eigen::Isometry3f()> fn);

    virtual void tick(float current_time_s, float dt) override;

    void set_perspective(Eigen::Matrix4f p);
    const Eigen::Matrix4f& get_perspective();

  private:
    // TODO: Include K matrix
    // float fov_y;
    // float aspect_ratio;
    // float near_distance;
    // float far_distance;
    control::CameraController camera_controller;

    Eigen::Matrix4f perspective;

    std::function<Eigen::Isometry3f()> tick_behavior_fn;
};
}  // namespace actor
