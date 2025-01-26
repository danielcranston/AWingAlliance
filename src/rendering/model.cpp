#include "rendering/model.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "data_handling.h"

namespace rendering
{
namespace
{
class LoadedModelData
{
  public:
    /**
     * @brief RAII wrapper around an assimp aiScene
     */
    LoadedModelData(const std::string& uri)
    {
        std::cout << "Loading \"" << uri << "\" ..." << std::endl;

        scene = importer.ReadFile(
            std::string(data_handling::MODELS_PATH) + uri,
            aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_FlipUVs |
                aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_OptimizeMeshes |
                aiProcess_RemoveRedundantMaterials | aiProcess_ImproveCacheLocality |
                aiProcess_OptimizeGraph | aiProcess_GenBoundingBoxes);

        if (!scene)
        {
            throw std::runtime_error("IMPORT FAILED: " + std::string(importer.GetErrorString()));
        }
    }

    std::vector<std::pair<aiMesh*, aiMaterial*>> get_mesh_data() const
    {
        std::vector<std::pair<aiMesh*, aiMaterial*>> out;
        out.reserve(scene->mNumMeshes);
        for (std::size_t i = 0; i < scene->mNumMeshes; i++)
        {
            aiMesh* aimesh = scene->mMeshes[i];
            aiMaterial* aimaterial = scene->mMaterials[aimesh->mMaterialIndex];
            out.push_back(std::make_pair(aimesh, aimaterial));
        }
        return out;
    }

  private:
    Assimp::Importer importer;  // Note: The importers destructor deletes the aiScene
    const aiScene* scene;
};

std::vector<float> parse_mesh_vertices(const aiMesh& aimesh)
{
    std::vector<float> vertices;
    vertices.reserve(3 * aimesh.mNumVertices);

    for (std::size_t k = 0; k < aimesh.mNumVertices; ++k)
    {
        vertices.push_back(aimesh.mVertices[k].x);
        vertices.push_back(aimesh.mVertices[k].y);
        vertices.push_back(aimesh.mVertices[k].z);
    }

    return vertices;
}

std::vector<float> parse_mesh_normals(const aiMesh& aimesh)
{
    std::vector<float> normals;
    normals.reserve(3 * aimesh.mNumVertices);

    for (std::size_t k = 0; k < aimesh.mNumVertices; ++k)
    {
        normals.push_back(aimesh.mNormals[k].x);
        normals.push_back(aimesh.mNormals[k].y);
        normals.push_back(aimesh.mNormals[k].z);
    }

    return normals;
}

std::vector<unsigned int> parse_mesh_indices(const aiMesh& aimesh)
{
    std::vector<unsigned int> indices;
    indices.reserve(3 * aimesh.mNumFaces);

    for (std::size_t k = 0; k < aimesh.mNumFaces; ++k)
    {
        const aiFace f = aimesh.mFaces[k];
        if (f.mNumIndices == 3)
        {
            for (int l = 0; l < 3; ++l)
            {
                indices.push_back(f.mIndices[l]);
            }
        }
    }

    return indices;
}

std::vector<float> parse_mesh_texture_coords(const aiMesh& aimesh)
{
    std::vector<float> texture_coords;
    texture_coords.reserve(2 * aimesh.mNumVertices);

    for (std::size_t k = 0; k < aimesh.mNumVertices; ++k)
    {
        texture_coords.push_back(aimesh.mTextureCoords[0][k].x);
        texture_coords.push_back(aimesh.mTextureCoords[0][k].y);
    }

    return texture_coords;
}

std::optional<std::string> parse_mesh_diffuse_texname(const aiMaterial& aimaterial)
{
    if (aimaterial.GetTextureCount(aiTextureType::aiTextureType_DIFFUSE) == 1)
    {
        aiString path;
        aimaterial.GetTexture(aiTextureType_DIFFUSE, 0, &path);
        return path.data;
    }

    return std::nullopt;
}

}  // namespace

Mesh::Mesh(const std::string& model_uri,
           const aiMesh& aimesh,
           const aiMaterial& aimaterial,
           const std::optional<TextureLoaderFn> texture_loader_fn)
{
    uri = model_uri + "_" + aimesh.mName.data;

    aabb = { Eigen::Vector3f(aimesh.mAABB.mMin.x, aimesh.mAABB.mMin.y, aimesh.mAABB.mMin.z),
             Eigen::Vector3f(aimesh.mAABB.mMax.x, aimesh.mAABB.mMax.y, aimesh.mAABB.mMax.z) };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo_vertices);
    glGenBuffers(1, &vbo_normals);
    glGenBuffers(1, &vbo_texture_coords);
    glGenBuffers(1, &vbo_indices);

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(3);

    {  // Vertices
        const auto& v = parse_mesh_vertices(aimesh);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);
        num_vertices = v.size() / 3;
    }

    {  // Normals
        const auto& n = parse_mesh_normals(aimesh);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
        glBufferData(GL_ARRAY_BUFFER, n.size() * sizeof(float), n.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);
    }

    {  // Texture coords
        const auto& tc = parse_mesh_texture_coords(aimesh);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_coords);
        glBufferData(GL_ARRAY_BUFFER, tc.size() * sizeof(float), tc.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0);
    }

    {  // Indices
        const auto& i = parse_mesh_indices(aimesh);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, i.size() * sizeof(uint), i.data(), GL_STATIC_DRAW);
        num_indices = i.size();
    }

    diffuse_texname = parse_mesh_diffuse_texname(aimaterial);
    if (texture_loader_fn)
    {
        texture = texture_loader_fn.value()(diffuse_texname);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
Mesh::Mesh(Mesh&& other) :
  uri(other.uri),
  aabb(other.aabb),
  num_vertices(other.num_vertices),
  num_indices(other.num_indices),
  diffuse_texname(other.diffuse_texname),
  vao(other.vao),
  vbo_vertices(other.vbo_vertices),
  vbo_indices(other.vbo_indices),
  vbo_normals(other.vbo_normals),
  vbo_texture_coords(other.vbo_texture_coords)
{
    std::cout << "Mesh \"" << uri << "\" (vao id " << vao << ") being moved" << std::endl;
    other.is_owning = false;
}

Mesh::~Mesh()
{
    if (is_owning)
    {
        std::cout << "Mesh \"" << uri << "\" (vao id " << vao << ") being cleaned up" << std::endl;
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo_vertices);
        glDeleteBuffers(1, &vbo_indices);
        glDeleteBuffers(1, &vbo_normals);
        glDeleteBuffers(1, &vbo_texture_coords);
    }
    else
    {
        std::cout << "  (Moved-from Mesh does not free any resources)" << std::endl;
    }
}

Model::Model(const std::string& uri, const std::optional<TextureLoaderFn> texture_loader_fn) :
  uri(uri)
{
    const auto model_data = LoadedModelData(uri);

    const std::vector<std::pair<aiMesh*, aiMaterial*>> mesh_data = model_data.get_mesh_data();

    meshes.reserve(mesh_data.size());
    texture_names.reserve(mesh_data.size());

    for (const auto& [aimesh, aimaterial] : mesh_data)
    {
        meshes.emplace_back(uri, *aimesh, *aimaterial, texture_loader_fn);
        if (meshes.back().diffuse_texname)
        {
            texture_names.push_back(meshes.back().diffuse_texname.value());
        }

        aabb = aabb.merged(meshes.back().aabb);
    }
}

Model::~Model()
{
    // std::cout << "Model Destructor" << std::endl;
}

}  // namespace rendering
