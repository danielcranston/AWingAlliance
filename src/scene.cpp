#include "scene.h"
#include <iostream>

#include "resources/locator.h"
#include "resources/load_descriptions.h"
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

Scene::Scene() : descriptions(resources::load_descriptions())
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

void Scene::register_ship(const std::string& name,
                          const std::string& type,
                          const Eigen::Vector3f& position,
                          const Eigen::Quaternionf& orientation)
{
    auto desc = descriptions.find(type);
    if (desc == descriptions.end())
    {
        throw std::runtime_error(std::string("Ship type not in descriptions: ") + type);
    }

    auto fc = FighterComponent();
    for (const auto& offset : desc->second.primary_fire_control.offsets)
    {
        fc.laser_spawn_points.emplace_back(
            geometry::make_pose({ offset[0], offset[1], offset[2] }));
    }

    for (int i = 0; i < desc->second.primary_fire_control.fire_modes.size(); ++i)
    {
        fc.fire_modes.emplace_back(desc->second.primary_fire_control.fire_modes[i],
                                   desc->second.primary_fire_control.recharge_times_s[i]);
    }

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
}

void Scene::register_camera(const Eigen::Matrix4f& perspective)
{
    current_camera_uid = registry.create();

    resource_manager.update_shaders(
        [&perspective](entt::resource_handle<const rendering::ShaderProgram> program) {
            program->use();
            program->setUniformMatrix4fv("perspective", perspective);
        });

    registry.emplace<MotionStateComponent>(current_camera_uid);
    registry.emplace<CameraComponent>(current_camera_uid, perspective);
}

std::shared_ptr<Scene> Scene::load_from_scenario(const std::string& scenario_name)
{
    std::cout << "building Scene" << std::endl;

    YAML::Node node = YAML::LoadFile(resources::locator::ROOT_PATH + scenario_name + ".yaml");

    auto ret = std::make_shared<Scene>();

    for (const auto& actor_node : node["ships"])
    {
        auto fighter_component = FighterComponent();
        if (actor_node["description"])
        {
        }

        ret->register_ship(actor_node["name"].as<std::string>(),
                           actor_node["description"].as<std::string>(),
                           to_vec3(actor_node["position"]),
                           to_quat(actor_node["orientation"]));
    }

    for (const auto& camera_node : node["cameras"])
    {
        ret->register_camera(
            geometry::perspective(M_PI / 180.0f * camera_node["intrinsics"]["fov_y"].as<float>(),
                                  camera_node["intrinsics"]["screen_w"].as<float>() /
                                      camera_node["intrinsics"]["screen_h"].as<float>(),
                                  camera_node["intrinsics"]["near"].as<float>(),
                                  camera_node["intrinsics"]["far"].as<float>()));
    }

    const auto skybox_uri = node["skybox"].as<std::string>();
    ret->resource_manager.load_skybox(skybox_uri);
    ret->registry.emplace<SkyboxComponent>(ret->registry.create(),
                                           ret->resource_manager.get_texture(skybox_uri),
                                           ret->resource_manager.get_model("box"));

    return ret;
}
