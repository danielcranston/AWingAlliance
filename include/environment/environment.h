#pragma once

#include <map>
#include <list>
#include <type_traits>

#include "actor/actor.h"
#include "actor/ship.h"
#include "actor/camera.h"
#include "actor/laser.h"

namespace environment
{
class Environment
{
  public:
    Environment();

    template <typename ActorType,
              typename = std::enable_if_t<std::is_base_of_v<actor::Actor, ActorType>>>
    void register_actor(ActorType&& actor)
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

    template <typename ActorType,
              typename = std::enable_if_t<std::is_base_of_v<actor::Actor, ActorType>>>
    ActorType& get_actor(const std::string& name)
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

    std::map<std::string, actor::Actor>& get_actors();
    std::map<std::string, actor::Ship>& get_ships();
    std::map<std::string, actor::Camera>& get_cameras();
    std::list<actor::Laser>& get_lasers();
    std::vector<std::string> get_visuals();

    void integrate(float current_time_s, float dt);

  private:
    std::map<std::string, actor::Actor> actors;
    std::map<std::string, actor::Ship> ships;
    std::map<std::string, actor::Camera> cameras;
    std::list<actor::Laser> lasers;

    // Ensures that actor 'name' exists/doesn't exist in 'map' ()
    template <typename ActorType>
    void assert_existence(std::map<std::string, ActorType>& map,
                          const std::string& name,
                          bool expected)
    {
        bool found = map.find(name) != map.end();
        if (found != expected)
        {
            // Mangled typename is better than nothing i guess ...
            std::string str = found ? "already exists" : "does not exist";
            throw std::runtime_error("Actor of type " + std::string(typeid(ActorType).name()) +
                                     " named '" + name + "' " + str +
                                     " in its respective container");
        }
    }
};
}  // namespace environment
