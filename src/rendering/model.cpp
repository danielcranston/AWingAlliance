#include <string>
#include <iostream>

#include "rendering/model.h"

namespace rendering
{
Model::Model(const std::string& name, std::vector<Mesh>&& mesh_vec)
  : name(name),
    min({ std::numeric_limits<float>::max(),
          std::numeric_limits<float>::max(),
          std::numeric_limits<float>::max() }),
    max({ std::numeric_limits<float>::min(),
          std::numeric_limits<float>::min(),
          std::numeric_limits<float>::min() })
{
    for (auto& m : mesh_vec)
    {
        meshes.emplace_back(std::move(m));

        for (int i = 0; i < 3; ++i)
        {
            min[i] = std::min(min[i], m.get_min()[i]);
            max[i] = std::max(max[i], m.get_max()[i]);
        }
    }

    std::cout << "Model " << name << " created.\n";
    std::cout << "  meshes.size(): " << meshes.size() << std::endl;
    std::cout << "  min: " << min[0] << " " << min[1] << " " << min[2] << std::endl;
    std::cout << "  max: " << max[0] << " " << max[1] << " " << max[2] << std::endl;
    std::cout << "  extents: " << max[0] - min[0] << " " << max[1] - min[1] << " "
              << max[2] - min[2] << std::endl;

    bounding_box = { Eigen::Vector3f(min[0], min[1], min[2]),
                     Eigen::Vector3f(max[0], max[1], max[2]) };
}

Model::~Model()
{
    std::cout << "Model Destructor" << std::endl;
}

const std::string& Model::get_name() const
{
    return name;
}

const std::vector<Mesh>& Model::get_meshes() const
{
    return meshes;
}

const Eigen::AlignedBox3f& Model::get_bounding_box() const
{
    return bounding_box;
}

}  // namespace rendering
