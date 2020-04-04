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
    ScenarioParser() {};
    ScenarioParser(std::string filename);
    void Parse();
    void PrintRequiredModels();

    std::string file_to_parse;


    struct ActorEntry
    {
        std::string type;
        glm::vec3 pos;
        glm::vec3 dir;
    };

    struct TerrainEntry
    {
        std::int32_t x;
        std::int32_t z;
        std::int32_t maxHeight;
        std::int32_t blockSize;
        std::int32_t seed;
        std::string heightmap;
        std::vector<std::string> textures;
    };

    struct SquadronEntry
    {
        std::string actor;
        glm::vec3 pos;
        glm::vec3 dir;
        std::int32_t members;
        std::string team;
    };

    std::set<std::string> required_models;
    std::map<std::string, ActorEntry> actors;
    std::map<std::string, SquadronEntry> squadrons;
    
    std::string player;
    std::string skybox;
    std::unique_ptr<TerrainEntry> terrain;
};