#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>

#include <terrain.h>
#include <PerlinNoise.hpp>

Terrain::Terrain(const ScenarioParser::TerrainEntry* terrainentry)
  : width{ terrainentry->x },
    length{ terrainentry->z },
    max_height{ terrainentry->maxHeight },
    block_size{ terrainentry->blockSize }
{
    double frequency = 1 + static_cast<int>(width / 64) * 2;  // std::clamp(frequency, 0.1, 64.0);
    int octaves = 4;                                          // std::clamp(octaves, 1, 16);

    const siv::PerlinNoise perlin(1);
    const double fx = width / frequency;
    const double fz = length / frequency;

    double Min = std::numeric_limits<double>::max();
    double Max = 0;

    // Heightmap
    for (int z = 0; z < length + 1; ++z)
    {
        for (int x = 0; x < width + 1; ++x)
        {
            double h = perlin.octaveNoise0_1(x / fx, z / fz, octaves);
            Min = std::min(Min, h);
            Max = std::max(Max, h);
            heightmap.push_back(h);
        }
    }
    // Scale height to [0, 1]
    for (int i = 0; i < heightmap.size(); ++i)
        heightmap[i] = (heightmap[i] - Min) / (Max - Min);

    // Normalmap
    for (int z = 0; z < length + 1; ++z)
    {
        for (int x = 0; x < width + 1; x++)
        {
            glm::vec3 normal = glm::vec3(0.0, 0.0, 0.0);

            float o = HeightAt(x, z);

            // normalized height difference between current and adjacent vertices
            float n = (HeightAt(x, z + 1) - o) * block_size;
            float s = (HeightAt(x, z - 1) - o) * block_size;
            float e = (HeightAt(x + 1, z) - o) * block_size;
            float w = (HeightAt(x - 1, z) - o) * block_size;

            float ne = (HeightAt(x + 1, z + 1) - o) * block_size;
            float se = (HeightAt(x + 1, z - 1) - o) * block_size;
            float nw = (HeightAt(x - 1, z + 1) - o) * block_size;
            float sw = (HeightAt(x - 1, z - 1) - o) * block_size;
            glm::vec3 normal1 = glm::cross(glm::vec3(0.0, n, 1.0), glm::vec3(1.0, ne, 1.0));
            glm::vec3 normal2 = glm::cross(glm::vec3(1.0, ne, 1.0), glm::vec3(1.0, e, 0.0));

            glm::vec3 normal3 = glm::cross(glm::vec3(1.0, e, 0.0), glm::vec3(0.0, s, -1.0));

            glm::vec3 normal4 = glm::cross(glm::vec3(0.0, s, -1.0), glm::vec3(-1.0, sw, -1.0));
            glm::vec3 normal5 = glm::cross(glm::vec3(-1.0, se, -1.0), glm::vec3(-1.0, w, 0.0));

            glm::vec3 normal6 = glm::cross(glm::vec3(-1.0, w, 0.0), glm::vec3(0.0, n, 1.0));

            normal = glm::normalize((normal1 + normal2 + normal3 + normal4 + normal5 + normal6));
            assert(normal.y > 0 && normal.y < 1);

            normalmap.push_back(normal);
        }
    }
}

float Terrain::HeightAt(int x, int z)
{
    x = (x + width) % width;  // ensure wraparound
    z = (z + length) % length;
    return max_height * heightmap[z * (width + 1) + x];
}

std::pair<float, glm::vec3> Terrain::GetHeight(float x, float z)
{
    float idx_x = (x / block_size + width / 2.0);
    float idx_z = (z / block_size + length / 2.0);

    int x0 = std::floor(idx_x);
    int z0 = std::floor(idx_z);
    float h0 = HeightAt(x0, z0);

    float interp_x = idx_x - x0;  // [0 1]
    float interp_z = idx_z - z0;  // [0 1]

    glm::vec3 v_x, v_z, v_temp;
    v_temp = glm::vec3(block_size, HeightAt(x0 + 1, z0 + 1) - h0, block_size);

    if (interp_x < interp_z)
    {
        v_z = glm::vec3(0.0, HeightAt(x0, z0 + 1) - h0, block_size);
        v_x = v_temp - v_z;
    }
    else
    {
        v_x = glm::vec3(block_size, HeightAt(x0 + 1, z0) - h0, 0.0);
        v_z = v_temp - v_x;
    }

    float height = (h0 + interp_x * v_x + interp_z * v_z).y;
    glm::vec3 normal = glm::normalize(glm::cross(v_z, v_x));

    return std::make_pair(height, normal);
}

std::pair<std::vector<float>, std::vector<uint>> Terrain::CreateBuffers() const
{
    std::vector<float> buffer;
    buffer.reserve(width * length * (3 + 3 + 2));
    for (int z = 0; z < length + 1; z++)
    {
        for (int x = 0; x < width + 1; x++)
        {
            // Vertex Position
            buffer.push_back(block_size * (-(width / 2.0) + x));
            buffer.push_back(max_height * (heightmap[z * (width + 1) + x]));
            buffer.push_back(block_size * (-(length / 2.0) + z));

            // Vertex Normal
            glm::vec3 normal = normalmap[z * (width + 1) + x];
            buffer.push_back(normal.x);
            buffer.push_back(normal.y);
            buffer.push_back(normal.z);

            // Vertex Tex Coord
            buffer.push_back(x * 1.0);
            buffer.push_back(z * 1.0);
        }
    }

    std::vector<unsigned int> indices;
    indices.reserve(width * length * 6);
    for (int z = 0; z < length; z++)
    {
        for (int x = 0; x < width; x++)
        {
            indices.push_back(z * (width + 1) + x);
            indices.push_back((z + 1) * (width + 1) + x);
            indices.push_back((z + 1) * (width + 1) + x + 1);

            indices.push_back(z * (width + 1) + x + 1);
            indices.push_back((z) * (width + 1) + x);
            indices.push_back((z + 1) * (width + 1) + x + 1);
        }
    }
    return std::make_pair(buffer, indices);
}

// Write to .bmp file stuff

#pragma pack(push, 1)
struct BMPHeader
{
    std::uint16_t bfType;
    std::uint32_t bfSize;
    std::uint16_t bfReserved1;
    std::uint16_t bfReserved2;
    std::uint32_t bfOffBits;
    std::uint32_t biSize;
    std::int32_t biWidth;
    std::int32_t biHeight;
    std::uint16_t biPlanes;
    std::uint16_t biBitCount;
    std::uint32_t biCompression;
    std::uint32_t biSizeImage;
    std::int32_t biXPelsPerMeter;
    std::int32_t biYPelsPerMeter;
    std::uint32_t biClrUsed;
    std::uint32_t biClrImportant;
};
static_assert(sizeof(BMPHeader) == 54);
#pragma pack(pop)

static constexpr std::uint8_t ToUint8(double x) noexcept
{
    return x >= 1.0 ? 255 : x <= 0.0 ? 0 : static_cast<std::uint8_t>(x * 255.0 + 0.5);
}

bool Terrain::saveBMP(const std::string& path)
{
    const std::int32_t rowSize = width * 3 + width % 4;
    const std::uint32_t bmpsize = rowSize * length;
    const BMPHeader header = { 0x4d42,
                               static_cast<std::uint32_t>(bmpsize + sizeof(BMPHeader)),
                               0,
                               0,
                               sizeof(BMPHeader),
                               40,
                               width,
                               length,
                               1,
                               24,
                               0,
                               bmpsize,
                               0,
                               0,
                               0,
                               0 };

    if (std::ofstream ofs{ path, std::ios_base::binary })
    {
        ofs.write((const char*)&header, sizeof(header));

        std::vector<std::uint8_t> line(rowSize);

        for (std::int32_t y = length - 1; - 1 < y; --y)
        {
            size_t pos = 0;

            for (std::int32_t x = 0; x < width; ++x)
            {
                const std::uint8_t col = ToUint8(heightmap[y * (width + 1) + x]);
                line[pos++] = col;
                line[pos++] = col;
                line[pos++] = col;
            }

            ofs.write((const char*)line.data(), line.size());
        }

        return true;
    }
    else
    {
        return false;
    }
};