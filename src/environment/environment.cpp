#include "environment/environment.h"

#include "geometry/geometry.h"
#include "resources/locator.h"
#include "resources/load_descriptions.h"

#include "yaml-cpp/yaml.h"

namespace environment
{
Environment::Environment()
{
}

std::shared_ptr<Environment> Environment::load_from_scenario(const std::string& scenario_name)
{
    auto descriptions = resources::load_descriptions();

    YAML::Node node = YAML::LoadFile(resources::locator::ROOT_PATH + scenario_name + ".yaml");

    auto ret = std::make_shared<Environment>();
    for (const auto& actor_node : node["actors"])
    {
        ret->register_actor<actor::Actor>(
            actor::Actor(actor_node["name"].as<std::string>(),
                         Eigen::Vector3f(actor_node["position"][0].as<float>(),
                                         actor_node["position"][1].as<float>(),
                                         actor_node["position"][2].as<float>()),
                         Eigen::Quaternionf(actor_node["orientation"][0].as<float>(),
                                            actor_node["orientation"][1].as<float>(),
                                            actor_node["orientation"][2].as<float>(),
                                            actor_node["orientation"][3].as<float>()),
                         actor_node["visual"].as<std::string>()));
    }

    for (const auto& ship_node : node["ships"])
    {
        ret->register_actor<actor::Ship>(
            actor::Ship(ship_node["name"].as<std::string>(),
                        descriptions.at(ship_node["description"].as<std::string>()),
                        Eigen::Vector3f(ship_node["position"][0].as<float>(),
                                        ship_node["position"][1].as<float>(),
                                        ship_node["position"][2].as<float>()),
                        Eigen::Quaternionf(ship_node["orientation"][0].as<float>(),
                                           ship_node["orientation"][1].as<float>(),
                                           ship_node["orientation"][2].as<float>(),
                                           ship_node["orientation"][3].as<float>())));
    }

    for (const auto& camera_node : node["cameras"])
    {
        auto perspective =
            geometry::perspective(M_PI / 180.0f * camera_node["intrinsics"]["fov_y"].as<float>(),
                                  camera_node["intrinsics"]["screen_w"].as<float>() /
                                      camera_node["intrinsics"]["screen_h"].as<float>(),
                                  camera_node["intrinsics"]["near"].as<float>(),
                                  camera_node["intrinsics"]["far"].as<float>());

        ret->register_actor<actor::Camera>(actor::Camera(camera_node["name"].as<std::string>(),
                                                         Eigen::Vector3f(0.0f, 0.0f, 0.0f),
                                                         Eigen::Quaternionf(1.0f, 0.0f, 0.0f, 0.0f),
                                                         perspective));
    }

    return ret;
}

std::map<std::string, actor::Actor>& Environment::get_actors()
{
    return actors;
}
std::map<std::string, actor::Ship>& Environment::get_ships()
{
    return ships;
}
std::map<std::string, actor::Camera>& Environment::get_cameras()
{
    return cameras;
}
std::list<actor::Laser>& Environment::get_lasers()
{
    return lasers;
}

std::vector<std::string> Environment::get_visuals()
{
    std::vector<std::string> out;

    for (const auto& [name, actor] : actors)
    {
        out.push_back(actor.get_visual_name());
    }

    for (const auto& [name, ship] : ships)
    {
        out.push_back(ship.get_visual_name());
    }

    return out;
}

void Environment::integrate(float current_time_s, float dt)
{
    for (auto& actor : actors)
    {
        actor.second.tick(current_time_s, dt);
    }
    for (auto& ship : ships)
    {
        ship.second.tick(current_time_s, dt);
    }
    for (auto& camera : cameras)
    {
        camera.second.tick(current_time_s, dt);
    }
    for (auto& laser : lasers)
    {
        laser.tick(current_time_s, dt);
    }
}

// Ensures that actor 'name' exists/doesn't exist in 'map' ()
template <typename ActorType>
void Environment::assert_existence(std::map<std::string, ActorType>& map,
                                   const std::string& name,
                                   bool expected)
{
    bool found = map.find(name) != map.end();
    if (found != expected)
    {
        // Mangled typename is better than nothing i guess ...
        std::string str = found ? "already exists" : "does not exist";
        throw std::runtime_error("Actor of type " + std::string(typeid(ActorType).name()) +
                                 " named '" + name + "' " + str + " in its respective container");
    }
}

template <typename ActorType,
          typename = std::enable_if_t<std::is_base_of_v<actor::Actor, ActorType>>>
void Environment::register_actor(ActorType&& actor)
{
    auto name = actor.get_name();
    if constexpr (std::is_same_v<ActorType, actor::Actor>)
    {
        assert_existence(actors, name, false);
        actors.insert(std::make_pair(name, std::move(actor)));
    }
    else if constexpr (std::is_same_v<ActorType, actor::Ship>)
    {
        assert_existence(ships, name, false);
        ships.insert(std::make_pair(name, std::move(actor)));
    }
    else if constexpr (std::is_same_v<ActorType, actor::Camera>)
    {
        assert_existence(cameras, name, false);
        cameras.insert(std::make_pair(name, std::move(actor)));
    }
    else if constexpr (std::is_same_v<ActorType, actor::Laser>)
    {
        lasers.emplace_front(actor);
    }
    else
    {
        static_assert(std::is_same_v<ActorType, int>, "How best to forcefail at compile time?");
    }
}

template void Environment::register_actor<actor::Actor>(actor::Actor&&);
template void Environment::register_actor<actor::Ship>(actor::Ship&&);
template void Environment::register_actor<actor::Laser>(actor::Laser&&);
template void Environment::register_actor<actor::Camera>(actor::Camera&&);

template <typename ActorType,
          typename = std::enable_if_t<std::is_base_of_v<actor::Actor, ActorType>>>
ActorType& Environment::get_actor(const std::string& name)
{
    if constexpr (std::is_same_v<ActorType, actor::Actor>)
    {
        assert_existence(actors, name, true);
        return actors.at(name);
    }
    else if constexpr (std::is_same_v<ActorType, actor::Ship>)
    {
        assert_existence(ships, name, true);
        return ships.at(name);
    }
    else if constexpr (std::is_same_v<ActorType, actor::Camera>)
    {
        assert_existence(cameras, name, true);
        return cameras.at(name);
    }
    else
    {
        static_assert(std::is_same_v<ActorType, int>, "How best to forcefail at compile time?");
    }
}

template actor::Actor& Environment::get_actor<actor::Actor>(const std::string&);
template actor::Ship& Environment::get_actor<actor::Ship>(const std::string&);
template actor::Camera& Environment::get_actor<actor::Camera>(const std::string&);

}  // namespace environment
