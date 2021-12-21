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
    void register_actor(ActorType&& actor);

    template <typename ActorType,
              typename = std::enable_if_t<std::is_base_of_v<actor::Actor, ActorType>>>
    ActorType& get_actor(const std::string& name);

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
                          bool expected);
};

extern template void Environment::register_actor<actor::Actor>(actor::Actor&&);
extern template void Environment::register_actor<actor::Ship>(actor::Ship&&);
extern template void Environment::register_actor<actor::Laser>(actor::Laser&&);
extern template void Environment::register_actor<actor::Camera>(actor::Camera&&);

extern template actor::Actor& Environment::get_actor<actor::Actor>(const std::string&);
extern template actor::Ship& Environment::get_actor<actor::Ship>(const std::string&);
extern template actor::Camera& Environment::get_actor<actor::Camera>(const std::string&);

}  // namespace environment
