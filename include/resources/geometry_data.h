#pragma once

#include <optional>
#include <set>
#include <map>
#include <vector>
#include <array>

namespace resources
{
struct GeometryData
{
    enum class Type
    {
        UNKNOWN,
        BOUNDINGBOX,
        CONVEXHULL
    };

    GeometryData() = default;
    GeometryData(const std::string& name) : name(name)
    {
    }

    std::string name;
    Type type;

    // BOUNDINGBOX
    std::optional<std::array<float, 3>> min;
    std::optional<std::array<float, 3>> max;

    // CONVEXHULL
    std::vector<float> vertices;
    std::vector<std::tuple<int, int, int>> face_indices;
    std::set<std::tuple<int, int>> edge_indices;

    std::map<std::string, GeometryData> children;
};
}  // namespace resources
