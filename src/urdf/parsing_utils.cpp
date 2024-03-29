#include "urdf/parsing_utils.h"

#include <algorithm>
#include <regex>
#include <iterator>
#include <sstream>

using namespace tinyxml2;

namespace
{
// https://stackoverflow.com/a/9437426
std::vector<std::string> split(const std::string& input, const std::string& regex)
{
    std::regex re(regex);
    std::sregex_token_iterator first = { input.begin(), input.end(), re, -1 };
    std::sregex_token_iterator last;
    return { first, last };
}

}  // namespace

namespace urdf::parsing_utils
{
void assert_nonempty_numeric(const std::string& val_string)
{
    const std::string err_msg = "String assertion failed: ";
    if (val_string.empty())
    {
        throw std::runtime_error(std::string(err_msg) + "received empty string");
    }

    for (const char c : val_string)
    {
        if (std::isalpha(c) || c == ',')
        {
            throw std::runtime_error(std::string(err_msg).append("") + val_string);
        }
    }
}

std::string parse_string_attribute(const XMLElement* element,
                                   const std::string& attrib_name,
                                   const bool empty_ok)
{
    if (std::string out = element->Attribute(attrib_name.c_str()); !out.empty())
    {
        return out;
    }
    else if (empty_ok)
    {
        return std::string("");
    }
    throw std::runtime_error(std::string("Failed to parse string attribute: ") + attrib_name);
}

float parse_float_attribute(const XMLElement* element, const std::string& attrib_name)
{
    auto val_string = element->Attribute(attrib_name.c_str());
    assert_nonempty_numeric(val_string);

    return std::stof(val_string);
}

Eigen::Vector3f parse_vec3_attribute(const XMLElement* element, const std::string& attrib_name)
{
    auto val_string = element->Attribute(attrib_name.c_str());
    assert_nonempty_numeric(val_string);

    auto split_string = split(val_string, " ");
    std::vector<float> vals;
    std::transform(split_string.begin(),
                   split_string.end(),
                   std::back_inserter(vals),
                   [](const auto& substr) { return std::stof(substr); });

    if (vals.size() != 3)
    {
        std::runtime_error("Failed to parse 3-float attribute");
    }

    return Eigen::Vector3f(vals[0], vals[1], vals[2]);
}

Eigen::Quaternionf parse_rpy_attribute(const XMLElement* element, const std::string& attrib_name)
{
    Eigen::Vector3f vals = parse_vec3_attribute(element, attrib_name);

    return Eigen::AngleAxisf(vals.x(), Eigen::Vector3f::UnitX()) *
           Eigen::AngleAxisf(vals.y(), Eigen::Vector3f::UnitY()) *
           Eigen::AngleAxisf(vals.z(), Eigen::Vector3f::UnitZ());
}

std::string parse_mesh_filename(const XMLElement* base_link)
{
    if (const auto* visual = base_link->FirstChildElement("visual"))
    {
        if (const auto* geometry = visual->FirstChildElement("geometry"))
        {
            if (const auto* mesh = geometry->FirstChildElement("mesh"))
            {
                std::string full_path = parse_string_attribute(mesh, "filename");
                if (std::size_t start = full_path.find("meshes/"); start != std::string::npos)
                {
                    return full_path.substr(start + sizeof("meshes/") - 1);
                }
                else
                {
                    return full_path;
                }
            }
        }
    }
    throw std::runtime_error("Failed to parse mesh filename from base_link");
}

Eigen::Vector3f parse_collision_geometry_size(const XMLElement* base_link)
{
    if (const auto* visual = base_link->FirstChildElement("collision"))
    {
        if (const auto* geometry = visual->FirstChildElement("geometry"))
        {
            if (const auto* box = geometry->FirstChildElement("box"))
            {
                return parse_vec3_attribute(box, "size");
            }
        }
    }
    throw std::runtime_error("Failed to parse collision geometry size from base_link");
}

Eigen::Isometry3f parse_joint_isometry(const XMLElement* joint)
{
    try
    {
        if (const auto* origin = joint->FirstChildElement("origin"))
        {
            auto isometry = Eigen::Isometry3f(parse_rpy_attribute(origin, "rpy"));
            isometry.translation() = parse_vec3_attribute(origin, "xyz");
            return isometry;
        }
        else
        {
            throw std::runtime_error(std::string("Failed to parse joint pose: missing 'origin' "
                                                 "element"));
        }
    }
    catch (std::logic_error& e)
    {
        throw std::runtime_error(std::string("Failed to parse joint pose: ") + e.what());
    }
}

FighterModel::FireMode parse_fire_mode_element(const XMLElement* fire_mode)
{
    FighterModel::FireMode out;

    out.recharge_time = parse_float_attribute(fire_mode, "recharge_time");
    std::string type = parse_string_attribute(fire_mode, "type", true);

    if (std::strcmp(type.c_str(), "single") == 0)
    {
        out.type = FighterModel::FireMode::FireType::SINGLE;
    }
    else if (std::strcmp(type.c_str(), "dual") == 0)
    {
        out.type = FighterModel::FireMode::FireType::DUAL;
    }
    else
    {
        throw std::runtime_error(std::string("Unrecognized fire mode type: ") + type);
    }

    return out;
}

FighterModel::Sounds parse_sounds_element(const XMLElement* sounds)
{
    FighterModel::Sounds out;
    if (const auto* laser = sounds->FirstChildElement("laser"))
    {
        out.laser = parse_string_attribute(laser, "file", true);

        if (const auto* engine = sounds->FirstChildElement("engine"))
        {
            out.engine = parse_string_attribute(engine, "file", true);

            if (const auto* hit = sounds->FirstChildElement("hit"))
            {
                out.hit = parse_string_attribute(hit, "file", true);
                return out;
            }
        }
    }
    throw std::runtime_error("Failed to parse laser element");
}

FighterModel::LaserInfo parse_laser_element(const XMLElement* laser)
{
    FighterModel::LaserInfo out;
    out.damage = parse_float_attribute(laser, "damage");
    if (const auto* speed = laser->FirstChildElement("speed"))
    {
        out.speed = 1000.0f * parse_float_attribute(speed, "kmps");

        if (const auto* size = laser->FirstChildElement("size"))
        {
            out.size = parse_vec3_attribute(size, "xyz");

            if (const auto* color = laser->FirstChildElement("color"))
            {
                out.color = parse_vec3_attribute(color, "rgb");

                if (const auto* impact = laser->FirstChildElement("impact"))
                {
                    if (const auto* size = impact->FirstChildElement("size"))
                    {
                        out.impact_info.size = parse_vec3_attribute(size, "xyz");

                        if (const auto* duration = impact->FirstChildElement("duration"))
                        {
                            out.impact_info.duration = parse_float_attribute(duration, "s");
                            return out;
                        }
                    }
                }
            }
        }
    }
    throw std::runtime_error("Failed to parse laser element");
}

FighterModel::MotionLimits parse_motion_limits_element(const XMLElement* limits)
{
    FighterModel::MotionLimits out;

    auto parse_limit = [limits](const std::string& limit_name) -> float {
        if (const auto* limit = limits->FirstChildElement(limit_name.c_str()))
        {
            return parse_float_attribute(limit, "max");
        }
        throw std::runtime_error("Failed to parse motion limit: " + limit_name);
    };

    out.velocity = parse_limit("velocity");
    out.acceleration = parse_limit("acceleration");
    out.angular_velocity = parse_limit("angular_velocity");
    out.angular_acceleration = parse_limit("angular_acceleration");

    return out;
}

FighterModel::HealthInfo parse_health_element(const tinyxml2::XMLElement* health)
{
    FighterModel::HealthInfo out;
    out.shields_max = parse_float_attribute(health, "shields");
    out.hull_max = parse_float_attribute(health, "hull");

    return out;
}

}  // namespace urdf::parsing_utils
