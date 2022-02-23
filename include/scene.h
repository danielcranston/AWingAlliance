#pragma once

#include <memory>
#include <Eigen/Dense>
#include <entt/entt.hpp>

#include "resources/description_data.h"
#include "geometry/geometry.h"
#include "rendering/model.h"
#include "rendering/shader_program.h"
#include "control/camera_controller.h"
#include "ecs/resource_manager.h"

class Scene
{
  public:
    Scene();
    ~Scene() = default;
    static std::shared_ptr<Scene> load_from_scenario(const std::string& scenario_name);

    void register_ship(const std::string& name,
                       const std::string& type,
                       const Eigen::Vector3f& position,
                       const Eigen::Quaternionf& orientation);
    void register_camera(const Eigen::Matrix4f& perspective);
    void register_texture(const std::string& uri);

    entt::registry registry;
    ResourceManager resource_manager;

    entt::entity player_uid;

    entt::entity current_camera_uid;
    control::CameraController camera_controller;

  private:
    std::map<std::string, resources::ActorDescription> descriptions;
};

struct model_loader final : entt::resource_loader<model_loader, rendering::Model>
{
    std::shared_ptr<rendering::Model> load(const std::string& uri) const;
};

struct primitive_model_loader final
  : entt::resource_loader<primitive_model_loader, rendering::Model>
{
    std::shared_ptr<rendering::Model> load(std::function<rendering::Model()> load_fn) const;
};

struct texture_loader final : entt::resource_loader<texture_loader, rendering::Texture>
{
    std::shared_ptr<rendering::Texture> load(const std::string& uri,
                                             const bool as_cubemap = false) const;
};

struct shader_loader final : entt::resource_loader<shader_loader, rendering::ShaderProgram>
{
    std::shared_ptr<rendering::ShaderProgram> load(const std::string& uri,
                                                   const std::string& vert_filename,
                                                   const std::string& frag_filename) const;
};
