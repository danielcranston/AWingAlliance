#include <functional>

#include "game_state.h"
#include "actor/ship.h"
#include "keyboard.h"

std::unique_ptr<GameState>
GameState::Create(const std::map<std::string, std::unique_ptr<Model>>* models_ptr)
{
    return std::make_unique<GameState>(models_ptr);
}

GameState::GameState(const std::map<std::string, std::unique_ptr<Model>>* models_ptr)
  : models_ptr(models_ptr),
    spline(Spline::Waypoint({ -32.0f, 148.0f, -128.0f }, { 0.0f, 0.0f, 1.0f }, 512.0f),
           Spline::Waypoint({ 0.0f, 112.0f, 0.0f }, { -1.0f, 0.0f, .0f }, 512.0f)),
    current_time(0.0f)
{
    actor::Billboard::GetCameraPosFunc = std::bind(&actor::Camera::GetPosition, std::ref(camera));
}

void GameState::register_ships(const std::map<std::string, ScenarioParser::ActorEntry>& actors)
{
    for (const auto& actor_item : actors)
    {
        register_ship(actor_item.first, actor_item.second);
    }
}

void GameState::register_ship(const std::string& name, const ScenarioParser::ActorEntry& actorentry)
{
    const Model* model_ptr = models_ptr->at(actorentry.type).get();
    const actor::Team team = actor::team_map.at(actorentry.team);
    Ships.insert(std::make_pair(
        name,
        actor::Ship::Create(
            actorentry.pos,
            actorentry.dir,
            model_ptr,
            team,
            std::bind(Laser::RegisterLaser, std::ref(Lasers), std::placeholders::_1))));
}

void GameState::register_laser(const Laser& laser)
{
}

void GameState::register_terrain(const ScenarioParser::TerrainEntry* terrainentry)
{
    terrain = std::make_unique<Terrain>(terrainentry);
}

const Terrain* const GameState::GetTerrain() const
{
    return terrain.get();
}

const std::map<std::string, std::unique_ptr<actor::Ship>>& GameState::GetShips() const
{
    return Ships;
}

const std::list<Laser>& GameState::GetLasers() const
{
    return Lasers;
}

const std::list<actor::Billboard>& GameState::GetBillboards() const
{
    return Billboards;
}

actor::Camera& GameState::GetCamera() const
{
    return camera;
}

void GameState::SetCameraPlacementFunc(std::function<std::pair<glm::vec3, glm::vec3>()> func)
{
    actor::Camera::placement_func = func;
}

const float GameState::GetCurrentTime() const
{
    return current_time;
}

void GameState::register_player(const std::string& name)
{
    player_name = name;
}

void GameState::integrate(const float t, const float d_time)
{
    ProcessKeyboardInput(keyboardInfo);
    using namespace std::chrono;
    float dt = d_time;
    current_time += dt;

    if (!Lasers.empty())
    {
        auto time_until_expiration =
            duration_cast<seconds>(Lasers.front().expire_time - system_clock::now());

        if (time_until_expiration < milliseconds(0))
        {
            Lasers.pop_front();
        }
        for (Laser& laser : Lasers)
        {
            if (laser.alive)
            {
                laser.Update(dt);
                for (const auto& ship : Ships)
                {
                    if (ship.second->IsColliding(laser))
                    {
                        ship.second->TakeDamage(25);
                        Billboards.emplace_back(laser.GetPosition(),
                                                laser.GetDirection(),
                                                actor::Billboard::Type::CAMERA_FACING,
                                                0,
                                                current_time);
                        laser.alive = false;
                        break;
                    }
                }
            }
        }
    }

    if (!Billboards.empty())
    {
        if (current_time > Billboards.front().GetExpireTime())
        {
            Billboards.pop_front();
        }
        for (actor::Billboard& billboard : Billboards)
        {
            billboard.Update(dt);
        }
    }

    if (keyboardInfo.test(KeyboardMapping::SPACEBAR))
        Ships.at("awing1")->Fire();

    Ships.at("awing1")->Update(keyboardInfo, dt);
    Ships.at("tie2")->Follow(dt);

    if (Ships.at("tie2")->IsInRange(Ships.at("awing1")->GetPosition(), 21.0f))
    {
        Ships.at("tie2")->SetTarget(Ships.at("bomber1").get());
    }
    if (Ships.at("tie2")->IsInRange(Ships.at("bomber1")->GetPosition(), 21.0f))
    {
        Ships.at("tie2")->SetTarget(Ships.at("awing1").get());
    }

    camera.Update(dt);
}
