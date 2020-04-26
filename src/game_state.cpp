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
  : models_ptr(models_ptr)
{
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
    Ships.insert(std::make_pair(
        name,
        actor::Ship::Create(
            actorentry.pos,
            actorentry.dir,
            model_ptr,
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

const std::map<std::string, std::unique_ptr<actor::Actor>>& GameState::GetShips() const
{
    return Ships;
}

const std::list<Laser>& GameState::GetLasers() const
{
    return Lasers;
}

void GameState::register_player(const std::string& name)
{
    player_name = name;
}

void GameState::integrate(std::chrono::system_clock::time_point t,
                          std::chrono::duration<float> d_time)
{
    ProcessKeyboardInput(keyboardInfo);
    float dt = std::chrono::duration_cast<std::chrono::milliseconds>(d_time).count() / 1000.0f;

    if (!Lasers.empty())
    {
        using namespace std::chrono;
        auto time_until_expiration =
            duration_cast<seconds>(Lasers.front().expire_time - system_clock::now());

        if (time_until_expiration < milliseconds(0))
        {
            Lasers.pop_front();
        }
        for (Laser& laser : Lasers)
        {
            laser.Update(dt);
        }
    }

    if (keyboardInfo.test(KeyboardMapping::SPACEBAR))
        dynamic_cast<actor::Ship*>(Ships.at("awing1").get())->Fire();

    dynamic_cast<actor::Ship*>(Ships.at("awing1").get())->Update(keyboardInfo, dt);
    dynamic_cast<actor::Ship*>(Ships.at("tie2").get())->Follow(*Ships.at("awing1"), dt);

    std::cout << "Lasers.size(): " << Lasers.size() << std::endl;

    const glm::vec3& player_pos = Ships[player_name]->GetPosition();
    const glm::vec3& player_dir = Ships[player_name]->GetDirection();
    const glm::vec3& player_desired_dir =
        dynamic_cast<actor::Ship*>(Ships.at(player_name).get())->GetDesiredDir();

    glm::vec3 cam_pos, cam_dir;
    if (keyboardInfo.test(KeyboardMapping::Q))
    {
        cam_pos = player_pos + 20.0f * player_desired_dir + 5.0f * glm::vec3(0.0, 1.0, 0.0);
        cam_dir = player_pos - 20.0f * player_dir;
    }
    else
    {
        cam_pos = player_pos + 5.0f * player_desired_dir - 25.0f * player_dir +
                  5.0f * glm::vec3(0.0, 1.0, 0.0);
        cam_dir = player_pos + 20.0f * player_dir;
    }

    camMatrix = glm::lookAt(cam_pos, cam_dir, glm::vec3(0.0, 1.0, 0.0));
    projCamMatrix = projMatrix * camMatrix;
}
