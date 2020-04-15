#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <parser.h>

struct Terrain
{
    explicit Terrain(const ScenarioParser::TerrainEntry* terrainentry);

    std::pair<float, glm::vec3> GetHeight(float x, float z);
    float HeightAt(int x, int z);
    std::pair<std::vector<float>, std::vector<uint>> CreateBuffers() const;

    bool saveBMP(const std::string& path);

    std::vector<float> heightmap;
    std::vector<glm::vec3> normalmap;
    std::int32_t width, length, max_height, block_size;
};
