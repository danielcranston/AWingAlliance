#include <string>
#include <vector>

#include "urdf/parsing.h"
#include "urdf/parsing_utils.h"
#include "resources/locator.h"
#include "external/tinyxml2.h"

using resources::locator::URDF_PATH;
using namespace tinyxml2;

namespace urdf
{
FighterModel parse_fighter_urdf(const std::string& filename)
{
    using namespace parsing_utils;

    XMLDocument doc;
    doc.LoadFile((std::string(URDF_PATH) + filename).c_str());
    if (doc.ErrorID())
    {
        throw std::runtime_error(doc.ErrorStr());
    }
    const XMLElement* robot = doc.FirstChildElement("robot");

    auto out = FighterModel();

    // Links
    for (const XMLElement* link = robot->FirstChildElement("link"); link;
         link = link->NextSiblingElement("link"))
    {
        if (std::strcmp(link->Attribute("name"), "base_link") == 0)
        {
            out.visual_name = parse_mesh_filename(link);
            out.dimensions = parse_collision_geometry_size(link);
            break;
        }
    }
    if (out.visual_name.empty() || out.dimensions.isApprox(Eigen::Vector3f::Zero()))
    {
        throw std::runtime_error("URDF does not contain 'base_link' link");
    }

    // Joints
    for (const XMLElement* joint = robot->FirstChildElement("joint"); joint;
         joint = joint->NextSiblingElement("joint"))
    {
        if (std::string(joint->Attribute("name")).find("turret") != std::string::npos)
        {
            out.laser_spawn_poses.push_back(parse_joint_isometry(joint));
        }
        if (std::string(joint->Attribute("name")).find("exhaust") != std::string::npos)
        {
            out.exhaust_poses.push_back(parse_joint_isometry(joint));
        }
        if (std::string(joint->Attribute("name")).find("camera") != std::string::npos)
        {
            out.camera_poses.push_back(parse_joint_isometry(joint));
        }
    }

    // Laser
    if (const XMLElement* laser = robot->FirstChildElement("laser"))
    {
        out.laser_info = parse_laser_element(laser);
    }
    else
    {
        throw std::runtime_error("URDF does not contain 'laser' element");
    }

    // Sounds
    if (const XMLElement* sounds = robot->FirstChildElement("sounds"))
    {
        out.sounds = parse_sounds_element(sounds);
    }
    else
    {
        throw std::runtime_error("URDF does not contain 'sounds' element");
    }

    // Fire Modes
    for (const XMLElement* fire_mode = robot->FirstChildElement("fire_mode"); fire_mode;
         fire_mode = fire_mode->NextSiblingElement("fire_mode"))
    {
        out.fire_modes.push_back(parse_fire_mode_element(fire_mode));
    }

    return out;
}
}  // namespace urdf
