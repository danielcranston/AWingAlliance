#include <iostream>
#include <fstream>
#include <array>
#include <vector>

#include <external/json.hpp>

#include "resources/locator.h"

#include "resources/load_descriptions.h"

#include "yaml-cpp/yaml.h"

using resources::locator::ROOT_PATH;

namespace resources
{
std::map<std::string, ActorDescription> load_descriptions()
{
    YAML::Node description = YAML::LoadFile(ROOT_PATH + std::string("descriptions.yaml"));

    std::map<std::string, ActorDescription> ret;
    for (YAML::const_iterator it = description.begin(); it != description.end(); ++it)
    {
        const auto name = it->first.as<std::string>();
        YAML::Node fire_control_node = it->second["fire_control"];

        std::vector<std::array<float, 3>> offsets;
        for (const auto& offset : fire_control_node["offsets"])
        {
            offsets.push_back(
                { offset[0].as<float>(), offset[1].as<float>(), offset[2].as<float>() });
        }

        std::vector<int> modes;
        std::vector<float> recharge_times_s;
        for (int i = 0; i < fire_control_node["modes"].size(); ++i)
        {
            modes.push_back(fire_control_node["modes"][i].as<int>());
            recharge_times_s.push_back(fire_control_node["recharge_times_s"][i].as<float>());
        }

        ret.insert({ name,
                     ActorDescription(
                         name,
                         it->second["type"].as<std::string>(),
                         it->second["visual"].as<std::string>(),
                         ActorDescription::FireControl(
                             std::move(offsets), std::move(modes), std::move(recharge_times_s))) });
    }
    return ret;
}
}  // namespace resources