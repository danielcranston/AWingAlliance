#pragma once

#include <memory>
#include <Eigen/Dense>
#include <entt/entt.hpp>

#include "control/camera_controller.h"
#include "control/ship_controller.h"
#include "ecs/resource_manager.h"

namespace ecs
{
class Scene
{
  public:
    Scene();
    ~Scene() = default;

    entt::entity register_ship(const std::string& name,
                               const std::string& urdf_filename,
                               const Eigen::Vector3f& position,
                               const Eigen::Quaternionf& orientation);
    entt::entity register_camera(const Eigen::Matrix4f& perspective);
    entt::entity register_laser(const Eigen::Vector3f& position,
                                const Eigen::Quaternionf& orientation,
                                const entt::resource<const urdf::FighterModel> model,
                                const Eigen::Vector3f& size,
                                const Eigen::Vector3f color,
                                const float speed,
                                entt::entity producer);
    entt::entity register_billboard(const Eigen::Vector3f& position,
                                    const Eigen::Quaternionf& orientation,
                                    const Eigen::Vector3f& size,
                                    const float birth_time,
                                    const float duration);
    entt::entity register_skybox(const std::string& skybox_uri);
    entt::entity register_sound_effect(const std::string buffer_name,
                                       const Eigen::Vector3f,
                                       const Eigen::Quaternionf& orientation);
    entt::entity register_spline(const Eigen::Vector3f& c0,
                                 const Eigen::Vector3f& c1,
                                 const Eigen::Vector3f& c2,
                                 const Eigen::Vector3f& c3);

    entt::registry registry;
    ecs::ResourceManager resource_manager;  // Could be made shared_ptr to allow scenes to share
                                            // same mgr

    entt::entity player_uid = entt::null;
    entt::entity camera_uid = entt::null;
    control::CameraController camera_controller;
    control::ShipController ship_controller;
};
}  // namespace ecs
