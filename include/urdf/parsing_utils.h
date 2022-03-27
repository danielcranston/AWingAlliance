#pragma once

#include <string>
#include <Eigen/Geometry>

#include "urdf/fighter_model.h"
#include "external/tinyxml2.h"

namespace urdf::parsing_utils
{
std::string parse_mesh_filename(const tinyxml2::XMLElement* base_link);
Eigen::Vector3f parse_collision_geometry_size(const tinyxml2::XMLElement* base_link);
Eigen::Isometry3f parse_joint_isometry(const tinyxml2::XMLElement* joint);
FighterModel::FireMode parse_fire_mode_element(const tinyxml2::XMLElement* fire_mode);
FighterModel::Sounds parse_sounds_element(const tinyxml2::XMLElement* sounds);
FighterModel::LaserInfo parse_laser_element(const tinyxml2::XMLElement* laser);
FighterModel::MotionLimits parse_motion_limits_element(const tinyxml2::XMLElement* limits);
FighterModel::HealthInfo parse_health_element(const tinyxml2::XMLElement* health);

void assert_nonempty_numeric(const std::string& val_string);
std::string parse_string_attribute(const tinyxml2::XMLElement* element,
                                   const std::string& attrib_name,
                                   const bool empty_ok = false);
float parse_float_attribute(const tinyxml2::XMLElement* element, const std::string& attrib_name);
Eigen::Vector3f parse_vec3_attribute(const tinyxml2::XMLElement* element,
                                     const std::string& atrib_name);
Eigen::Quaternionf parse_rpy_attribute(const tinyxml2::XMLElement* element,
                                       const std::string& attrib_name);

}  // namespace urdf::parsing_utils
