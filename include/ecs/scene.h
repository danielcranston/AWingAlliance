#pragma once

#include <memory>
#include <Eigen/Dense>
#include <entt/entt.hpp>

#include "control/camera_controller.h"
#include "ecs/resource_manager.h"

namespace ecs
{
class Scene
{
  public:
    Scene();
    ~Scene() = default;

    entt::entity register_ship(const std::string& name,
                               const std::string& type,
                               const Eigen::Vector3f& position,
                               const Eigen::Quaternionf& orientation);
    entt::entity register_camera(const Eigen::Matrix4f& perspective);

    entt::registry registry;
    ResourceManager resource_manager;  // Could be made shared_ptr to allow scenes to share same mgr

    entt::entity player_uid = entt::null;

    entt::entity camera_uid = entt::null;
    control::CameraController camera_controller;
};
}  // namespace ecs
