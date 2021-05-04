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

}  // namespace environment