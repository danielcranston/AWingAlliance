#pragma once

#include <array>

#include "resources/texture_data.h"

namespace resources
{
TextureData load_texture(const std::string& filename);
std::array<TextureData, 6> load_cubemap_texture(const std::string& folder_name);
}  // namespace resources
