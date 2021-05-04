#pragma once

#include "resources/mesh_data.h"

namespace resources
{
std::vector<MeshData> load_model(const std::string& filename);
}  // namespace resources
