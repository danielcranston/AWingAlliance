#pragma once

#include "resources/mesh_data.h"

namespace resources
{
//
enum class LoadingMode
{
    VISUAL = 1,   // Stricter loading flags to optimize mode
    GEOMETRY = 2  // Less strict, preserving mesh structure (hierarchy)
};

std::vector<MeshData> load_model(const std::string& filename,
                                 const LoadingMode mode = LoadingMode::VISUAL);
}  // namespace resources
