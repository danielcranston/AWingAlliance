#include "rendering/primitives.h"

namespace rendering::primitives
{
namespace
{
}
Model quad()
{
    const std::vector<float> vertices = { -1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
                                          1.0f,  -1.0f, 0.0f, 1.0f,  1.0f, 0.0f };
    const std::vector<float>& normals = vertices;
    const std::vector<float> texture_coords = { 0.0f, 0.0f, 0.0f, 1.0f,  //
                                                1.0f, 0.0f, 1.0f, 1.0f };
    const std::vector<uint>& indices = { 0, 1, 2, 2, 1, 3 };
    const std::array<float, 3>& min = { -1.0f, -1.0f, 0.0f };
    const std::array<float, 3>& max = { 1.0f, 1.0f, 0.0f };
    std::vector<Mesh> m;
    m.emplace_back(Mesh("quad",
                        std::move(vertices),
                        std::move(normals),
                        std::move(texture_coords),
                        std::move(indices),
                        std::move(min),
                        std::move(max),
                        ""));
    return Model(m[0].get_name(), std::move(m));
}

Model box()
{
    const std::vector<float> vertices = { -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,  //
                                          1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,  //
                                          -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,   //
                                          1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f };

    const std::vector<float>& normals = vertices;
    const std::vector<float> texture_coords = { 0.0f, 0.0f, 1.0f, 0.0f,  //
                                                1.0f, 1.0f, 0.0f, 1.0f,  //
                                                0.0f, 0.0f, 1.0f, 0.0f,  //
                                                1.0f, 1.0f, 0.0f, 1.0f };

    const std::vector<uint>& indices = { 0, 1, 3, 3, 1, 2,  //
                                         1, 5, 2, 2, 5, 6,  //
                                         5, 4, 6, 6, 4, 7,  //
                                         4, 0, 7, 7, 0, 3,  //
                                         3, 2, 7, 7, 2, 6,  //
                                         4, 5, 0, 0, 5, 1 };

    const std::array<float, 3>& min = { -1.0f, -1.0f, -1.0f };
    const std::array<float, 3>& max = { 1.0f, 1.0f, 1.0f };
    std::vector<Mesh> m;
    m.emplace_back(Mesh("box",
                        std::move(vertices),
                        std::move(normals),
                        std::move(texture_coords),
                        std::move(indices),
                        std::move(min),
                        std::move(max),
                        ""));
    return Model(m[0].get_name(), std::move(m));
};

Model bounding_box()
{
    const std::vector<float> vertices = { -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f,  //
                                          0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f,  //
                                          -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,   //
                                          0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f };

    const std::vector<float>& normals = vertices;
    const std::vector<float> texture_coords = { 0.0f, 0.0f, 1.0f, 0.0f,  //
                                                1.0f, 1.0f, 0.0f, 1.0f,  //
                                                0.0f, 0.0f, 1.0f, 0.0f,  //
                                                1.0f, 1.0f, 0.0f, 1.0f };

    const std::vector<uint>& indices = { 0, 1, 1, 2, 2, 3, 3, 0,  //
                                         4, 5, 5, 6, 6, 7, 7, 4,  //
                                         0, 4, 1, 5, 2, 6, 3, 7 };

    const std::array<float, 3>& min = { -1.0f, -1.0f, -1.0f };
    const std::array<float, 3>& max = { 1.0f, 1.0f, 1.0f };

    std::vector<Mesh> m;
    m.emplace_back(Mesh("bounding_box",
                        std::move(vertices),
                        std::move(normals),
                        std::move(texture_coords),
                        std::move(indices),
                        std::move(min),
                        std::move(max),
                        ""));
    return Model(m[0].get_name(), std::move(m));
};
}  // namespace rendering::primitives
