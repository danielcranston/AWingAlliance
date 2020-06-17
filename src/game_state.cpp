#include <functional>

#include "game_state.h"
#include "actor/ship.h"
#include "keyboard.h"
#include "behavior/ship_controller.h"

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
    // Register various callbacks
    actor::Billboard::GetCameraPosFunc = std::bind(&actor::Camera::GetPosition, std::ref(camera));
    actor::Ship::RegisterLaserFunc = [this](const actor::Laser& new_laser) {
        this->Lasers.push_back(new_laser);
    };
    ShipController::GetTargetFunc = [this](const actor::Ship& requester) -> const actor::Ship* {
        // Go through all Ships and choose the closest enemy ship
        const actor::Team& team = requester.GetTeam();

        float closest_dist = std::numeric_limits<float>::max();
        actor::Ship* closest_ship = nullptr;
        for (const auto& item : Ships)
        {
            if (item.second->GetTeam() != team)
            {
                const float dist =
                    std::abs(glm::distance(item.second->GetPosition(), requester.GetPosition()));
                if (dist < closest_dist)
                {
                    closest_dist = dist;
                    closest_ship = item.second.get();
                }
            }
        }
        return closest_ship;
    };
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
        name, actor::Ship::Create(actorentry.pos, actorentry.dir, name, model_ptr, team)));
}

void GameState::register_laser(const actor::Laser& laser)
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

const std::list<actor::Laser>& GameState::GetLasers() const
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
        for (actor::Laser& laser : Lasers)
        {
            if (laser.alive)
            {
                laser.Update(dt);
                for (const auto& ship : Ships)
                {
                    if (ship.second->IsColliding(laser))
                    {
                        ship.second->TakeDamage(25);
                        Billboards.emplace_back(laser.GetPosition() +
                                                    laser.GetDirection() * laser.GetLength(),
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

    for (const auto& ship : Ships)
    {
        if (ship.second.get() != Ships.at("awing1").get())
            ship.second->Update(dt);
    }

    Ships.at("awing1")->Update(keyboardInfo, dt);

    camera.Update(dt);
}
