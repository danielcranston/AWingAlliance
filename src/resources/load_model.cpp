#include <iostream>

#include <assimp/Importer.hpp>   // C++ importer interface
#include <assimp/scene.h>        // Output data structure
#include <assimp/postprocess.h>  // Post processing flags

#include "resources/locator.h"
#include "resources/load_model.h"
#include "resources/mesh_data.h"

using resources::locator::MODELS_PATH;

namespace resources
{
namespace
{
MeshData process_mesh_data(const std::string& filename, const int mesh_num, const aiScene* scene)
{
    const aiMesh* m = scene->mMeshes[mesh_num];

    if (!m->HasPositions())
        throw std::runtime_error("Loading " + filename +
                                 ": expected positions, but none were found");
    if (!m->HasFaces())
        throw std::runtime_error("Loading " + filename +
                                 ": expected faces (indices), but none were found");
    if (!m->HasNormals())
        throw std::runtime_error("Loading " + filename + ": expected normals, but none were found");
    if (!m->HasTextureCoords(0))
        throw std::runtime_error("Loading " + filename +
                                 ": expected texture coordinates, but none were found");

    MeshData buffer;
    buffer.name = filename + "_" + m->mName.data;
    buffer.vertices.reserve(3 * m->mNumVertices);
    buffer.normals.reserve(3 * m->mNumVertices);
    buffer.indices.reserve(3 * m->mNumFaces);
    buffer.texture_coords.reserve(2 * m->mNumVertices);

    for (int k = 0; k < m->mNumVertices; ++k)
    {
        buffer.vertices.push_back(m->mVertices[k].x);
        buffer.vertices.push_back(m->mVertices[k].y);
        buffer.vertices.push_back(m->mVertices[k].z);

        // Calculate min/max extents for each mesh
        if (m->mVertices[k].x < buffer.min[0])
            buffer.min[0] = m->mVertices[k].x;
        if (m->mVertices[k].y < buffer.min[1])
            buffer.min[1] = m->mVertices[k].y;
        if (m->mVertices[k].z < buffer.min[2])
            buffer.min[2] = m->mVertices[k].z;

        if (m->mVertices[k].x > buffer.max[0])
            buffer.max[0] = m->mVertices[k].x;
        if (m->mVertices[k].y > buffer.max[1])
            buffer.max[1] = m->mVertices[k].y;
        if (m->mVertices[k].z > buffer.max[2])
            buffer.max[2] = m->mVertices[k].z;
    }
    for (int k = 0; k < m->mNumVertices; ++k)
    {
        buffer.normals.push_back(m->mNormals[k].x);
        buffer.normals.push_back(m->mNormals[k].y);
        buffer.normals.push_back(m->mNormals[k].z);
    }
    for (int k = 0; k < m->mNumVertices; ++k)
    {
        buffer.texture_coords.push_back(m->mTextureCoords[0][k].x);
        buffer.texture_coords.push_back(m->mTextureCoords[0][k].y);
    }
    for (int k = 0; k < m->mNumFaces; ++k)
    {
        const aiFace f = m->mFaces[k];
        if (f.mNumIndices != 3)
        {
            // std::cout << "Loading " << filename << " face " << k
            //           << ": expected 3 indices in face, but found " << f.mNumIndices <<
            //           std::endl;
            continue;
        }
        for (int l = 0; l < 3; ++l)
        {
            buffer.indices.push_back(f.mIndices[l]);
        }
    }

    const aiMaterial* mat = scene->mMaterials[m->mMaterialIndex];
    const int num_diffuse_textures = mat->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE);

    if (num_diffuse_textures > 1)
    {
        // std::cout << "Loading \"" << filename << "\": expected max 1 diffuse texture for mesh"
        //           << " but found " << num_diffuse_textures;
    }

    if (num_diffuse_textures == 1)
    {
        aiString path;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        buffer.diffuse_texname = path.data;
    }

    // std::cout << "  MeshData \"" << buffer.name << "\" (\"" << buffer.diffuse_texname << "\")"
    //           << std::endl;

    return buffer;
}

void process_node(const std::string& filename,
                  const aiNode* node,
                  const aiScene* scene,
                  std::vector<MeshData>& buffers)
{
    // process all the node's meshes (if any)
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        buffers.push_back(process_mesh_data(filename, node->mMeshes[i], scene));
    }

    // then do the same for each of its children
    for (int i = 0; i < node->mNumChildren; ++i)
    {
        process_node(filename, node->mChildren[i], scene, buffers);
    }
}
}  // namespace

std::vector<MeshData> load_model(const std::string& filename)
{
    // std::cout << "Loading \"" << filename << "\"" << std::endl;

    Assimp::Importer importer;
    const aiScene* scene =
        importer.ReadFile(std::string(MODELS_PATH) + filename,
                          aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_FlipUVs |
                              aiProcess_JoinIdenticalVertices | aiProcess_SortByPType |
                              aiProcess_OptimizeMeshes | aiProcess_RemoveRedundantMaterials |
                              aiProcess_ImproveCacheLocality | aiProcess_OptimizeGraph);
    // aiProcess_PreTransformVertices not to be used with OptimizeSceneGraph

    if (!scene)
        throw std::runtime_error("IMPORT FAILED: " + std::string(importer.GetErrorString()));

    const aiNode* root = scene->mRootNode;

    if (!root->mTransformation.IsIdentity())
        throw std::runtime_error("Loading " + filename +
                                 ": root node does not have identity transformation");

    std::vector<MeshData> buffers;
    process_node(filename, scene->mRootNode, scene, buffers);

    return buffers;
}
}  // namespace resources