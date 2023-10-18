#include "yaml-cpp/yaml.h"

#include "ecs/scene_factory.h"
#include "ecs/components.h"
#include "geometry/geometry.h"
#include "resources/locator.h"

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
}  // namespace

namespace ecs
{
std::shared_ptr<Scene> SceneFactory::create_from_scenario(const std::string& scenario_name)
{
    YAML::Node node = YAML::LoadFile(resources::locator::ROOT_PATH + scenario_name + ".yaml");

    auto ret = std::make_shared<Scene>();

    for (const auto& actor_node : node["ships"])
    {
        auto entity = ret->register_ship(actor_node["name"].as<std::string>(),
                                         actor_node["urdf_filename"].as<std::string>(),
                                         to_vec3(actor_node["position"]),
                                         to_quat(actor_node["orientation"]));

        if (actor_node["player"] && actor_node["player"].as<bool>())
        {
            ret->player_uid = entity;
        }
        else
        {
            auto context = std::make_shared<sm::RoamingStateMachineContext>();
            ret->registry.emplace<RoamingStateMachineComponent>(entity, context);
        }
    }
    if (ret->player_uid == entt::null)
    {
        throw std::runtime_error("Scenario file does not specify which ship is the player");
    }

    for (const auto& camera_node : node["cameras"])
    {
        auto entity = ret->register_camera(
            geometry::perspective(M_PI / 180.0f * camera_node["intrinsics"]["fov_y"].as<float>(),
                                  camera_node["intrinsics"]["screen_w"].as<float>() /
                                      camera_node["intrinsics"]["screen_h"].as<float>(),
                                  camera_node["intrinsics"]["near"].as<float>(),
                                  camera_node["intrinsics"]["far"].as<float>()));

        if (camera_node["active"] && camera_node["active"].as<bool>())
        {
            ret->camera_uid = entity;
        }
    }
    if (ret->camera_uid == entt::null)
    {
        throw std::runtime_error("Scenario file does not specify a main/active camera");
    }

    ret->register_skybox(node["skybox"].as<std::string>());

    return ret;
}
}  // namespace ecs
