#include "environment/environment.h"

namespace environment
{
Environment::Environment()
{
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
