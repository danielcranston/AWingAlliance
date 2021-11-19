#include <algorithm>
#include <iostream>
#include <string>
#include <map>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "resources/locator.h"

using resources::locator::MODELS_PATH;

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
    std::cout << "  names: | ";
    for (const auto& name : names)
    {
        std::cout << name << " | ";
    }
    std::cout << std::endl;
}

struct VolumeNode
{
    VolumeNode() = default;
    VolumeNode(const std::string& name, const std::optional<int>& data) : name(name), data(data)
    {
    }

    std::string name;
    std::optional<int> data;
    std::map<std::string, VolumeNode> children;
};

void add_node(const aiNode* node, VolumeNode& volume_tree)
{
    std::cout << "Processing node " << node->mName.C_Str() << std::endl;
    std::cout << "  num_children: " << node->mNumChildren << std::endl;
    std::cout << "  num_meshes: " << node->mNumMeshes << std::endl;

    auto names = segment_name(node->mName.C_Str());
    print_names(names);

    auto* cur_volume_node = &volume_tree;

    if (node->mNumChildren)
    {
        for (int i = 0; i < node->mNumChildren; ++i)
        {
            add_node(node->mChildren[i], volume_tree);
        }
    }
    else
    {
        for (const auto& name : names)
        {
            if (cur_volume_node->children.find(name) == cur_volume_node->children.end())
                cur_volume_node->children[name] = VolumeNode(name, std::nullopt);

            cur_volume_node = &(cur_volume_node->children[name]);
        }

        std::cout << "    final name is " << cur_volume_node->name << std::endl;

        cur_volume_node->data = 1;
    }
}

VolumeNode test_assimp(const std::string& filename)
{
    Assimp::Importer importer;
    const aiScene* scene =
        importer.ReadFile(std::string(MODELS_PATH) + filename,
                          aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_FlipUVs |
                              aiProcess_JoinIdenticalVertices | aiProcess_RemoveRedundantMaterials);

    std::cout << "scene->mNumMeshes: " << scene->mNumMeshes << std::endl;

    const aiNode* root = scene->mRootNode;

    VolumeNode volume_tree(root->mName.C_Str(), std::nullopt);
    add_node(root, volume_tree);

    return volume_tree;
}

int main(int argc, char** argv)
{
    auto a = test_assimp("test_col.obj");

    std::cout << "Done" << std::endl;
    std::cout << a.name << std::endl;
    for (const auto& item : a.children)
    {
        std::cout << "  " << item.first << " \t|  has data: " << item.second.data.has_value()
                  << std::endl;
        for (const auto& item2 : item.second.children)
        {
            std::cout << "    " << item2.first
                      << " \t|  has data: " << item2.second.data.has_value() << std::endl;
            for (const auto& item3 : item2.second.children)
            {
                std::cout << "      " << item3.first
                          << " \t|  has data: " << item3.second.data.has_value() << std::endl;
                for (const auto& item4 : item3.second.children)
                {
                    std::cout << "      " << item4.first
                              << " \t|  has data: " << item4.second.data.has_value() << std::endl;
                    for (const auto& item5 : item4.second.children)
                    {
                        std::cout << "      " << item5.first
                                  << " \t|  has data: " << item5.second.data.has_value()
                                  << std::endl;
                    }
                }
            }
        }
    }
}
