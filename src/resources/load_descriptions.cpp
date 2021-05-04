#include <iostream>
#include <fstream>
#include <array>
#include <vector>

#include <ryml_std.hpp>
#include <ryml.hpp>

#include <external/json.hpp>

#include "resources/locator.h"
#include "resources/load_textfile.h"

#include "resources/load_descriptions.h"

using resources::locator::ROOT_PATH;

namespace resources
{
namespace
{
std::string to_string(const c4::csubstr& substr)
{
    return std::string(substr.str, substr.len);
}
}  // namespace

std::map<std::string, ActorDescription> load_descriptions()
{
    std::string src = resources::load_textfile("descriptions.yaml", ROOT_PATH);

    c4::substr srcview = c4::to_substr(src.data());
    ryml::Tree tree = ryml::parse(srcview);
    ryml::NodeRef rootref = tree.rootref();

    std::cout << "rootref num children: " << rootref.num_children() << std::endl;

    std::map<std::string, ActorDescription> ret;
    for (ryml::NodeRef c : rootref.children())
    {
        std::cout << "---" << c.key() << "\n";

        assert(c.has_child("type"));
        assert(c.has_child("visual"));
        assert(c.has_child("fire_control"));

        // Fire Control
        ryml::NodeRef fire_control_node = c["fire_control"];

        assert(fire_control_node.has_child("offsets"));
        assert(fire_control_node.has_child("modes"));

        std::vector<std::array<float, 3>> offsets;
        for (int i = 0; i < fire_control_node["offsets"].num_children(); ++i)
        {
            offsets.push_back({ std::stof(fire_control_node["offsets"].child(i)[0].val().str),
                                std::stof(fire_control_node["offsets"].child(i)[1].val().str),
                                std::stof(fire_control_node["offsets"].child(i)[2].val().str) });
        }

        assert(fire_control_node["modes"].num_children() ==
               fire_control_node["recharge_times_s"].num_children());

        std::vector<int> modes;
        std::vector<float> recharge_times_s;
        for (int i = 0; i < fire_control_node["modes"].num_children(); ++i)
        {
            int a = std::stoi(fire_control_node["modes"].child(i).val().str);
            float b = std::stof(fire_control_node["recharge_times_s"].child(i).val().str);

            modes.push_back(a);
            recharge_times_s.push_back(b);
        }

        ret.insert(std::make_pair(
            to_string(c.key()),
            ActorDescription(to_string(c.key()),
                             to_string(c["type"].val()),
                             to_string(c["visual"].val()),
                             ActorDescription::FireControl(std::move(offsets),
                                                           std::move(modes),
                                                           std::move(recharge_times_s)))));
    }
    return ret;
}
}  // namespace resources