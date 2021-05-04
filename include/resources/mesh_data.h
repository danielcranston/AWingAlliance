#pragma once

#include <string>
#include <vector>
#include <array>
#include <limits>

namespace resources
{
struct MeshData
{
    MeshData()
      : min({ std::numeric_limits<float>::max(),
              std::numeric_limits<float>::max(),
              std::numeric_limits<float>::max() }),
        max({ std::numeric_limits<float>::min(),
              std::numeric_limits<float>::min(),
              std::numeric_limits<float>::min() }){};

    std::string name;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texture_coords;
    std::vector<uint> indices;
    std::string diffuse_texname;
    std::array<float, 3> min;
    std::array<float, 3> max;
};
}  // namespace resources
