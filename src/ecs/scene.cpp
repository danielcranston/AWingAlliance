#include "ecs/scene.h"
#include <iostream>

#include "resources/load_model.h"
#include "resources/load_texture.h"
#include "rendering/primitives.h"
#include "rendering/compile_shader_program.h"
#include "ecs/components.h"

#include "yaml-cpp/yaml.h"

namespace
{
Eigen::Vector3f to_vec3(const YAML::Node& node)
{
    return Eigen::Vector3f(node[0].as<float>(), node[1].as<float>(), node[2].as<float>());
}

Eigen::Quaternionf to_quat(const YAML::Node& node)
{
    return Eigen::Quaternionf(
        node[0].as<float>(), node[1].as<float>(), node[2].as<float>(), node[3].as<float>());
}

std::ostream& operator<<(std::ostream& os, const Eigen::Quaternionf& q)
{
    os << q.x() << " " << q.y() << " " << q.z() << " " << q.w();
    return os;
}
}  // namespace

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
                                  const std::string& type,
                                  const Eigen::Vector3f& position,
                                  const Eigen::Quaternionf& orientation)
{
    const auto entity = registry.create();
    auto& motion_state = registry.emplace<MotionStateComponent>(entity);
    motion_state.position = position;
    motion_state.orientation = orientation;

    resource_manager.load_model(type + std::string(".obj"));
    auto model_handle = resource_manager.get_model(type + std::string(".obj"));

    auto texture_handles = std::vector<entt::resource_handle<const rendering::Texture>>();
    for (const auto& mesh : model_handle->get_meshes())
    {
        texture_handles.push_back(resource_manager.get_texture(mesh.get_texture_name()));
    }

    registry.emplace<VisualComponent>(entity, model_handle, texture_handles);

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

}  // namespace ecs
