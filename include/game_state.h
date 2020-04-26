#pragma once

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include <bitset>

#include "actor/actor.h"
#include "actor/laser.h"
#include "terrain.h"

class GameState
{
  public:
    static std::unique_ptr<GameState>
    Create(const std::map<std::string, std::unique_ptr<Model>>* models_ptr);
    explicit GameState(const std::map<std::string, std::unique_ptr<Model>>* models_ptr);

    void register_ships(const std::map<std::string, ScenarioParser::ActorEntry>& actors);
    void register_ship(const std::string& name, const ScenarioParser::ActorEntry& actorentry);
    void register_laser(const Laser& laser);
    void register_terrain(const ScenarioParser::TerrainEntry* terrainentry);
    void register_player(const std::string& name);

    void integrate(std::chrono::system_clock::time_point t, std::chrono::duration<float> d_time);

    const std::map<std::string, std::unique_ptr<actor::Actor>>& GetShips() const;
    const Terrain* const GetTerrain() const;
    const std::list<Laser>& GetLasers() const;

    glm::mat4 projCamMatrix, camMatrix, projMatrix;

  private:
    std::map<std::string, std::unique_ptr<actor::Actor>> Ships;
    std::list<Laser> Lasers;
    std::unique_ptr<Terrain> terrain;
    std::string player_name;
    const std::map<std::string, std::unique_ptr<Model>>* models_ptr;

    std::bitset<8> keyboardInfo = 0;

    std::chrono::system_clock::time_point t;
    const std::chrono::system_clock::duration dt = std::chrono::milliseconds(10);
};
