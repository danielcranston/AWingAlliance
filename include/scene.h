#pragma once

#include <memory>
#include <Eigen/Dense>
#include <entt/entt.hpp>

#include "resources/description_data.h"
#include "geometry/geometry.h"
#include "rendering/model.h"
#include "rendering/shader_program.h"

class Scene
{
  public:
    Scene();
    ~Scene();
    static std::shared_ptr<Scene> load_from_scenario(const std::string& scenario_name);

    void register_ship(const std::string& name,
                       const std::string& type,
                       const Eigen::Vector3f& position,
                       const Eigen::Quaternionf& orientation);
    void register_skybox(const std::string& uri);
    void register_camera(const Eigen::Matrix4f& perspective);
    void register_texture(const std::string& uri);

    entt::registry registry;
    entt::resource_cache<rendering::Model> model_cache;
    entt::resource_cache<rendering::Texture> texture_cache;
    entt::resource_cache<rendering::ShaderProgram> shader_cache;

    entt::entity player_uid;
    entt::entity current_camera_uid;

  private:
    std::map<std::string, resources::ActorDescription> descriptions;

    // Listeners
    void on_visual_added(entt::registry& registry, entt::entity entity);
    void on_skybox_added(entt::registry& registry, entt::entity entity);
    void on_camera_added(entt::registry& registry, entt::entity entity);
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
