#pragma once

#include <Eigen/Geometry>

#include "rendering/mesh.h"
#include "rendering/texture.h"

namespace rendering
{
class Model
{
  public:
    Model(Model&) = delete;
    Model(const Model&) = delete;
    Model& operator=(Model&) = delete;
    Model& operator=(const Model&) = delete;

    Model(const std::string& name, std::vector<Mesh>&& mesh_vec);
    Model(Model&& other) = default;
    ~Model();

    const std::string& get_name() const;
    const std::vector<Mesh>& get_meshes() const;
    const Eigen::AlignedBox3f& get_bounding_box() const;

  private:
    std::string name;
    std::array<float, 3> min;
    std::array<float, 3> max;
    Eigen::AlignedBox3f bounding_box;
    std::vector<Mesh> meshes;
};
}  // namespace rendering
