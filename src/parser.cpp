#include <iostream>
#include <fstream>

#include <json.hpp>
#include <parser.h>

ScenarioParser::ScenarioParser(const std::string& filename)
{
    file_to_parse = filename;

    required_models.clear();
    actors.clear();
    squadrons.clear();

    player = "";
    skybox = "";
}
ScenarioParser::ActorEntry::ActorEntry(const std::string& _type,
                                       const std::string& _team,
                                       const std::vector<float>& _pos,
                                       const std::vector<float>& _dir)
  : type(_type), team(_team), pos(glm::make_vec3(_pos.data())), dir(glm::make_vec3(_dir.data()))
{
}

ScenarioParser::TerrainEntry::TerrainEntry(const std::int32_t _x,
                                           const std::int32_t _z,
                                           const std::int32_t _maxHeight,
                                           const std::int32_t _blockSize,
                                           const std::int32_t _seed,
                                           const std::string& _heightmap,
                                           const std::vector<std::string>& _textures)
  : x(_x),
    z(_z),
    maxHeight(_maxHeight),
    blockSize(_blockSize),
    seed(_seed),
    heightmap(_heightmap),
    textures(_textures.begin(), _textures.end())
{
}

ScenarioParser::SquadronEntry::SquadronEntry(const std::string& _actor,
                                             const std::vector<float>& _pos,
                                             const std::vector<float>& _dir,
                                             const std::int32_t _members,
                                             const std::string& _team)
  : actor(_actor),
    pos(glm::make_vec3(_pos.data())),
    dir(glm::make_vec3(_dir.data())),
    members(_members),
    team(_team)
{
}

void ScenarioParser::Parse()
{
    std::cout << "Parsing scenario file [" << file_to_parse << "]\n";

    std::ifstream i(file_to_parse);
    nlohmann::json scenario;
    i >> scenario;

    if (scenario.find("actors") != scenario.end())
    {
        for (const auto& actor : scenario["actors"].items())
        {
            const std::string model_name = std::string(actor.value()["type"]);
            required_models.insert(model_name);

            const ActorEntry ae(
                model_name, actor.value()["team"], actor.value()["pos"], actor.value()["dir"]);

            actors.insert(std::make_pair(actor.key(), ae));
        }
    }
    std::cout << "  Number of actors: " << actors.size() << '\n';

    if (scenario.find("terrain") != scenario.end())
    {
        terrain = std::make_unique<TerrainEntry>(scenario["terrain"]["x"],
                                                 scenario["terrain"]["z"],
                                                 scenario["terrain"]["maxHeight"],
                                                 scenario["terrain"]["blockSize"],
                                                 scenario["terrain"]["seed"],
                                                 scenario["terrain"]["heightmap"],
                                                 scenario["terrain"]["textures"]);
    }
    std::cout << "  Use terrain? ";
    if (terrain)
        std::cout << "Yes\n";
    else
        std::cout << "No\n";

    if (scenario.find("skybox") != scenario.end())
    {
        skybox = scenario["skybox"];
    }
    std::cout << "  Skybox image: " << skybox << std::endl;

    if (!(scenario.find("player") != scenario.end()))
        throw std::runtime_error("player not specified in scenario file!");
    player = scenario["player"];
    std::cout << "  Player ID: " << player << std::endl;

    if (scenario.find("squadrons") != scenario.end())
    {
        for (const auto& squadron : scenario["squadrons"].items())
        {
            const std::string model_name = squadron.value()["actor"];
            required_models.insert(model_name);

            const SquadronEntry se(model_name,
                                   squadron.value()["pos"],
                                   squadron.value()["dir"],
                                   squadron.value()["members"],
                                   squadron.value()["team"]);
            squadrons.insert(std::make_pair(squadron.key(), se));
        }
    }
    std::cout << "  Number of squadrons: " << squadrons.size() << '\n';
    PrintRequiredModels();
};

void ScenarioParser::PrintRequiredModels()
{
    std::cout << "  Required models: " << std::endl;
    for (const auto& model_name : required_models)
        std::cout << "    " << model_name << std::endl;
}
