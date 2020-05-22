#pragma once

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include <bitset>

#include "actor/ship.h"
#include "actor/laser.h"
#include "actor/billboard.h"
#include "actor/camera.h"
#include "terrain.h"
#include "spline.h"

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

    const std::map<std::string, std::unique_ptr<actor::Ship>>& GetShips() const;
    const Terrain* const GetTerrain() const;
    const std::list<Laser>& GetLasers() const;
    const std::list<actor::Billboard>& GetBillboards() const;
    Camera& GetCamera() const;
    const float GetCurrentTime() const;

  private:
    std::map<std::string, std::unique_ptr<actor::Ship>> Ships;
    std::list<Laser> Lasers;
    std::list<actor::Billboard> Billboards;
    std::unique_ptr<Terrain> terrain;
    std::string player_name;
    const std::map<std::string, std::unique_ptr<Model>>* models_ptr;
    mutable Camera camera;

    Spline spline;

    std::bitset<8> keyboardInfo = 0;
    float current_time;
};
