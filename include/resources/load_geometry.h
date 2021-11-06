#pragma once

#include <string>

#include "resources/geometry_data.h"

namespace resources
{
GeometryData load_geometry(const std::string& filename);
}  // namespace resources
