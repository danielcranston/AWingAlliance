#include "ecs/scene.h"
#include <iostream>

#include "resources/load_model.h"
#include "resources/load_texture.h"
#include "rendering/primitives.h"
#include "rendering/compile_shader_program.h"
#include "ecs/components.h"

#include "yaml-cpp/yaml.h"

namespace ecs
{
Scene::Scene()
{
    resource_manager.load_shader("model", "model.vert", "model.frag");
    resource_manager.load_shader("skybox", "sky.vert", "sky.frag");

    resource_manager.update_shaders(
        [](entt::resource_handle<const rendering::ShaderProgram> program) {
            program->use();
            program->setUniform1i("tex", 0);
            program->setUniformMatrix4fv("model_scale", Eigen::Matrix4f::Identity());
        });
}

entt::entity Scene::register_ship(const std::string& name,
                                  const std::string& urdf_filename,
                                  const Eigen::Vector3f& position,
                                  const Eigen::Quaternionf& orientation)
{
    resource_manager.load_fighter_model(urdf_filename);
    auto fighter_model_handle = resource_manager.get_fighter_model(urdf_filename);

    resource_manager.load_model(fighter_model_handle->visual_name);
    auto model_handle = resource_manager.get_model(fighter_model_handle->visual_name);

    auto texture_handles = std::vector<entt::resource_handle<const rendering::Texture>>();
    for (const auto& mesh : model_handle->get_meshes())
    {
        texture_handles.push_back(resource_manager.get_texture(mesh.get_texture_name()));
    }

    const auto entity = registry.create();
    registry.emplace<FighterComponent>(entity, name, fighter_model_handle);
    registry.emplace<VisualComponent>(entity, model_handle, texture_handles);
    registry.emplace<MotionStateComponent>(entity, position, orientation);

    return entity;
}

entt::entity Scene::register_camera(const Eigen::Matrix4f& perspective)
{
    auto entity = registry.create();

    resource_manager.update_shaders(
        [&perspective](entt::resource_handle<const rendering::ShaderProgram> program) {
            program->use();
            program->setUniformMatrix4fv("perspective", perspective);
        });

    registry.emplace<MotionStateComponent>(entity);
    registry.emplace<CameraComponent>(entity, perspective);

    return entity;
}

entt::entity Scene::register_laser(const Eigen::Vector3f& position,
                                   const Eigen::Quaternionf& orientation,
                                   const entt::resource_handle<const urdf::FighterModel> model,
                                   const Eigen::Vector3f& size,
                                   const Eigen::Vector3f color,
                                   const float speed,
                                   entt::entity producer)
{
    auto entity = registry.create();
    auto& motion_state = registry.emplace<MotionStateComponent>(entity, position, orientation);
    motion_state.velocity = orientation * Eigen::Vector3f(speed, 0, 0);
    registry.emplace<LaserComponent>(entity, producer, model, size(0));
    registry.emplace<VisualComponent>(
        entity, resource_manager.get_model("box"), std::nullopt, color, size);

    return entity;
}

entt::entity Scene::register_billboard(const Eigen::Vector3f& position,
                                       const Eigen::Quaternionf& orientation,
                                       const Eigen::Vector3f& size,
                                       const float duration,
                                       const float birth_time)
{
    auto entity = registry.create();
    registry.emplace<MotionStateComponent>(entity, position, orientation);
    registry.emplace<BillboardComponent>(
        entity, geometry::to_scale_matrix(size), birth_time, duration);

    return entity;
}

}  // namespace ecs
