#pragma once

#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class ScenarioParser
{
  public:
    ScenarioParser(const std::string& filename);
    void Parse();
    void PrintRequiredModels();

    struct ActorEntry
    {
        explicit ActorEntry(const std::string& _type,
                            const std::string& _team,
                            const std::vector<float>& _pos,
                            const std::vector<float>& _dir);
        const std::string type;
        const std::string team;
        const glm::vec3 pos;
        const glm::vec3 dir;
    };

    struct TerrainEntry
    {
        explicit TerrainEntry(const std::int32_t _x,
                              const std::int32_t _z,
                              const std::int32_t _maxHeight,
                              const std::int32_t _blockSize,
                              const std::int32_t _seed,
                              const std::string& _heightmap,
                              const std::vector<std::string>& _textures);
        const std::int32_t x;
        const std::int32_t z;
        const std::int32_t maxHeight;
        const std::int32_t blockSize;
        const std::int32_t seed;
        const std::string heightmap;
        const std::vector<std::string> textures;
    };

    struct SquadronEntry
    {
        explicit SquadronEntry(const std::string& _actor,
                               const std::vector<float>& _pos,
                               const std::vector<float>& _dir,
                               const std::int32_t _members,
                               const std::string& _team);
        const std::string actor;
        const glm::vec3 pos;
        const glm::vec3 dir;
        const std::int32_t members;
        const std::string team;
    };

    std::string file_to_parse;

    std::set<std::string> required_models;
    std::map<std::string, ActorEntry> actors;
    std::map<std::string, SquadronEntry> squadrons;

    std::string player;
    std::string skybox;
    std::unique_ptr<TerrainEntry> terrain;
};