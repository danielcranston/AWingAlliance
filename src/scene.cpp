#include "scene.h"
#include <iostream>

#include "resources/locator.h"
#include "resources/load_descriptions.h"
#include "resources/load_model.h"
#include "resources/load_texture.h"
#include "rendering/primitives.h"
#include "rendering/compile_shader_program.h"

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

void Scene::on_visual_added(entt::registry& registry, entt::entity entity)
{
    if (auto visual = registry.get<VisualComponent>(entity).texture_uri;
        !model_cache.contains(visual))
    {
        model_cache.load<model_loader>(visual, visual.data());

        for (const auto& mesh : model_cache.handle(visual)->get_meshes())
        {
            if (mesh.get_texture_name().size())
            {
                auto texture_uri = entt::hashed_string(mesh.get_texture_name().c_str());
                if (!texture_cache.contains(texture_uri))
                {
                    texture_cache.load<texture_loader>(texture_uri, mesh.get_texture_name());
                }
            }
        }
    }
}

void Scene::on_skybox_added(entt::registry& registry, entt::entity entity)
{
    const auto skybox_component = registry.get<SkyboxComponent>(entity);

    if (auto texture_uri = skybox_component.texture_uri; !texture_cache.contains(texture_uri))
    {
        texture_cache.load<texture_loader>(texture_uri, texture_uri.data(), true);
    }
    if (auto model_uri = skybox_component.model_uri; !model_cache.contains(model_uri))
    {
        model_cache.load<primitive_model_loader>(model_uri, &rendering::primitives::box);
    }
}

void Scene::on_camera_added(entt::registry& registry, entt::entity entity)
{
    const auto camera_component = registry.get<CameraComponent>(entity);

    auto shader_setup_fn = [&camera_component](rendering::ShaderProgram& program) {
        program.use();
        program.setUniform1i("tex", 0);
        program.setUniformMatrix4fv("perspective", camera_component.perspective);
        program.setUniformMatrix4fv("model_scale", Eigen::Matrix4f::Identity());
    };

    shader_setup_fn(shader_cache.handle(entt::hashed_string("model")).get());
    shader_setup_fn(shader_cache.handle(entt::hashed_string("skybox")).get());
}

Scene::Scene() : descriptions(resources::load_descriptions())
{
    auto model_uri = entt::hashed_string("model");
    auto sky_uri = entt::hashed_string("skybox");
    shader_cache.load<shader_loader>(model_uri, model_uri.data(), "model.vert", "model.frag");
    shader_cache.load<shader_loader>(sky_uri, "sky", "sky.vert", "sky.frag");

    registry.on_construct<VisualComponent>().connect<&Scene::on_visual_added>(*this);
    registry.on_construct<SkyboxComponent>().connect<&Scene::on_skybox_added>(*this);
    registry.on_construct<CameraComponent>().connect<&Scene::on_camera_added>(*this);
}

Scene::~Scene()
{
    registry.on_construct<VisualComponent>().disconnect<&Scene::on_visual_added>(*this);
    registry.on_construct<SkyboxComponent>().disconnect<&Scene::on_skybox_added>(*this);
    registry.on_construct<CameraComponent>().disconnect<&Scene::on_camera_added>(*this);
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

    auto fc = FighterComponent{};
    std::transform(desc->second.primary_fire_control.offsets.begin(),
                   desc->second.primary_fire_control.offsets.end(),
                   std::back_inserter(fc.laser_spawn_points),
                   [](const std::array<float, 3>& offset) {
                       auto out = Eigen::Isometry3f::Identity();
                       out.translation() = Eigen::Vector3f(offset[0], offset[1], offset[2]);
                       return out;
                   });

    for (int i = 0; i < desc->second.primary_fire_control.fire_modes.size(); ++i)
    {
        fc.fire_modes.emplace_back(desc->second.primary_fire_control.fire_modes[i],
                                   desc->second.primary_fire_control.recharge_times_s[i]);
    }

    const auto entity = registry.create();
    auto& motion_state = registry.emplace<geometry::MotionState>(entity);
    motion_state.position = position;
    motion_state.orientation = orientation;

    registry.emplace<VisualComponent>(entity,
                                      entt::hashed_string(std::string(type + ".obj").c_str()));
}

void Scene::register_skybox(const std::string& uri)
{
    registry.emplace<SkyboxComponent>(
        registry.create(), entt::hashed_string(uri.c_str()), entt::hashed_string("box"));
}

void Scene::register_camera(const Eigen::Matrix4f& perspective)
{
    current_camera_uid = registry.create();

    registry.emplace<geometry::MotionState>(current_camera_uid);
    registry.emplace<CameraComponent>(current_camera_uid, perspective);
}

std::shared_ptr<Scene> Scene::load_from_scenario(const std::string& scenario_name)
{
    std::cout << "building Scene" << std::endl;

    YAML::Node node = YAML::LoadFile(resources::locator::ROOT_PATH + scenario_name + ".yaml");

    auto ret = std::make_shared<Scene>();

    for (const auto& actor_node : node["ships"])
    {
        auto fighter_component = FighterComponent{};
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

    ret->registry.emplace<SkyboxComponent>(
        ret->registry.create(), entt::hashed_string(node["skybox"].as<std::string>().c_str()));

    return ret;
}

std::shared_ptr<rendering::Model> model_loader::load(const std::string& uri) const
{
    auto meshes = std::vector<rendering::Mesh>();
    for (const auto& mesh_data : resources::load_model(uri))
    {
        meshes.emplace_back(mesh_data);
    }

    return std::make_shared<rendering::Model>(uri, std::move(meshes));
}

std::shared_ptr<rendering::Model>
primitive_model_loader::load(std::function<rendering::Model()> load_fn) const
{
    return std::make_shared<rendering::Model>(load_fn());
}

std::shared_ptr<rendering::Texture> texture_loader::load(const std::string& uri,
                                                         const bool as_cubemap) const
{
    if (as_cubemap)
    {
        return std::make_shared<rendering::Texture>(uri, resources::load_cubemap_texture(uri));
    }
    else
    {
        return std::make_shared<rendering::Texture>(resources::load_texture(uri));
    }
}

std::shared_ptr<rendering::ShaderProgram>
shader_loader::load(const std::string& uri,
                    const std::string& vert_filename,
                    const std::string& frag_filename) const
{
    return rendering::compileShaders(uri, vert_filename, frag_filename);
}
