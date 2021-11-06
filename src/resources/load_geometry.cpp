#include <algorithm>
#include <sstream>
#include <iostream>
#include <cassert>
#include <set>
#include <string>
#include <array>
#include <map>

#include "resources/locator.h"
#include "resources/load_geometry.h"
#include "resources/load_model.h"
#include "resources/mesh_data.h"

namespace resources
{
namespace
{
std::vector<std::string> segment_name(const std::string& name)
{
    std::vector<std::string> ret;

    std::stringstream ss(name);
    std::string segment;

    while (std::getline(ss, segment, '/'))
    {
        ret.push_back(segment);
    }

    return ret;
}

void print_names(const std::vector<std::string>& names)
{
    for (const auto& name : names)
    {
        std::cout << " | " << name;
    }
    std::cout << " |" << std::endl;
}

}  // namespace

void insert_data(const std::vector<std::string>& names, const MeshData& mesh, GeometryData& root)
{
    GeometryData* cur_node_ptr = &root;
    for (const auto& node_name : names)
    {
        if (cur_node_ptr->children.find(node_name) == cur_node_ptr->children.end())
        {
            cur_node_ptr->children[node_name] = GeometryData(node_name);
        }
        cur_node_ptr = &cur_node_ptr->children[node_name];
    }

    // Reached the end of the chain

    cur_node_ptr->min = mesh.min;
    cur_node_ptr->max = mesh.max;

    if (mesh.vertices.size() / 3 == 8)
    {
        // std::cout << "Adding bounding box: ";
        // print_names(names);
        cur_node_ptr->type = GeometryData::Type::BOUNDINGBOX;
    }
    else
    {
        // std::cout << "Adding convex hull with " << std::to_string(mesh.vertices.size() / 3)
        //           << " verts: ";
        // print_names(names);
        cur_node_ptr->type = GeometryData::Type::CONVEXHULL;
        cur_node_ptr->vertices = mesh.vertices;

        cur_node_ptr->face_indices.reserve(mesh.indices.size() / 3);

        assert(mesh.indices.size() % 3 == 0);

        for (int i = 0; i < mesh.indices.size(); i = i + 3)
        {
            cur_node_ptr->edge_indices.insert(
                std::make_pair(mesh.indices[i + 0], mesh.indices[i + 1]));
            cur_node_ptr->edge_indices.insert(
                std::make_pair(mesh.indices[i + 0], mesh.indices[i + 2]));
            cur_node_ptr->edge_indices.insert(
                std::make_pair(mesh.indices[i + 1], mesh.indices[i + 2]));

            cur_node_ptr->face_indices.push_back(
                std::make_tuple(mesh.indices[i + 0], mesh.indices[i + 1], mesh.indices[i + 2]));
        }
    }
}

GeometryData load_geometry(const std::string& filename)
{
    auto meshes = load_model(filename, LoadingMode::GEOMETRY);
    // std::cout << "loading " << filename << std::endl;

    auto ret = GeometryData(filename);

    for (const auto& mesh : meshes)
    {
        auto names = segment_name(mesh.name);
        insert_data(names, mesh, ret);
    }

    return ret;
}
}  // namespace resources
