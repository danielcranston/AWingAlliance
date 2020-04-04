#include <iostream>
#include <fstream>

#include <json.hpp>
#include <parser.h>

ScenarioParser::ScenarioParser(std::string filename)
{
    file_to_parse = filename;

    required_models.clear();
    actors.clear();
    squadrons.clear();

    player = "";
    skybox = "";
}

void ScenarioParser::Parse()
{
    std::cout << "Parsing scenario file [" << file_to_parse << "]\n";

    using json = nlohmann::json;
    std::ifstream i(file_to_parse);
    json scenario;
    i >> scenario;

    if(scenario.find("actors") != scenario.end())
    {
        for (auto& actor : scenario["actors"].items())
        {
            // std::cout << actor.key() << " : type=" << actor.value()["type"] << '\n';
            ActorEntry ae;
            
            std::string model_name = std::string(actor.value()["type"]);
            required_models.insert(model_name);

            std::vector<float> pos = actor.value()["pos"];
            std::vector<float> dir = actor.value()["dir"];
            
            ae.type = model_name;
            ae.pos = glm::make_vec3(&pos[0]);
            ae.dir = glm::make_vec3(&dir[0]);

            actors.insert(std::make_pair(actor.key(), ae));
        }
    }
    std::cout << "  Number of actors: " << actors.size() << '\n';

    if(scenario.find("terrain") != scenario.end())
    {
        terrain = std::make_unique<TerrainEntry>();
        terrain->x = scenario["terrain"]["x"];
        terrain->z = scenario["terrain"]["z"];
        terrain->maxHeight = scenario["terrain"]["maxHeight"];
        terrain->blockSize = scenario["terrain"]["blockSize"];
        terrain->seed = scenario["terrain"]["seed"];
        terrain->heightmap = scenario["terrain"]["heightmap"];
        for(auto tex_name : scenario["terrain"]["textures"])
        {
            terrain->textures.push_back(tex_name);
        }
    }
    std::cout << "  Use terrain? ";
    if(terrain) std::cout << "Yes\n";
    else std::cout << "Yes\n";

    if(scenario.find("skybox") != scenario.end())
    {
        skybox = scenario["skybox"];
    }
    std::cout << "  Skybox image: " << skybox << std::endl;

    if(scenario.find("player") != scenario.end())
    {
        player = scenario["player"];
    }
    std::cout << "  Player ID: " << player << std::endl;

    if(scenario.find("squadrons") != scenario.end())
    {
        for (auto& squadron : scenario["squadrons"].items())
        {
            SquadronEntry se;
    
            std::string model_name = std::string(squadron.value()["actor"]);
            required_models.insert(model_name);

            std::vector<float> pos = squadron.value()["pos"];
            std::vector<float> dir = squadron.value()["dir"];
            
            se.actor = model_name;
            se.pos = glm::make_vec3(&pos[0]);
            se.dir = glm::make_vec3(&dir[0]);
            se.members = squadron.value()["members"];
            se.team = squadron.value()["team"];

            squadrons.insert(std::make_pair(squadron.key(), se));
        }
    }
    std::cout << "  Number of squadrons: " << squadrons.size() << '\n';
    PrintRequiredModels();
};

void ScenarioParser::PrintRequiredModels()
{
    std::cout << "  Required models: " << std::endl;
    for(auto const& model_name : required_models)
        std:: cout << "    " << model_name << std::endl;
}
