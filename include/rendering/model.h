#pragma once

#include <optional>
#include <string>
#include <vector>

#include <Eigen/Geometry>

#include "rendering/texture.h"

class aiMesh;
class aiMaterial;

namespace rendering
{

using TextureLoaderFn =
    std::function<std::shared_ptr<Texture>(std::optional<const std::string> uri)>;

class Mesh
{
  public:
    Mesh(const std::string& model_uri,
         const aiMesh& aimesh,
         const aiMaterial& aimaterial,
         const std::optional<TextureLoaderFn> texture_loader_fn = std::nullopt);

    // Disallow copy constructor and copy assignment
    Mesh(Mesh&) = delete;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Move constructor transfers ownership to avoid early deallocation of the VBOs/VAO. Relevant:
    // https://www.khronos.org/opengl/wiki/Common_Mistakes#RAII_and_hidden_destructor_calls
    // Const versions are possible thanks to is_owning being mutable. Relevant:
    // https://www.reddit.com/r/cpp/comments/8wbeom/coding_guideline_avoid_const_member_variables/
    Mesh(Mesh&& other);

    // TODO: Implement if needed
    Mesh(const Mesh&& other) = delete;
    Mesh& operator=(Mesh&& other) = delete;
    Mesh& operator=(const Mesh&& other) = delete;

    ~Mesh();

    std::string uri;
    Eigen::AlignedBox3f aabb;
    int num_vertices;
    int num_indices;
    std::optional<std::string> diffuse_texname;  // Name of the texture defined in the model
    std::shared_ptr<Texture> texture;            // For convenience

    uint vao = 0;
    uint vbo_vertices = 0;
    uint vbo_indices = 0;
    uint vbo_normals = 0;
    uint vbo_texture_coords = 0;

  private:
    mutable bool is_owning = true;
};

/**
 * @brief Since I can't make up my mind on whether Models (Meshes) should own their respective
 * Textures or not, this allows the user to choose (at the cost of some mental overhead)
 */
constexpr auto DontLoadTexture = std::nullopt;
constexpr auto AutoLoadTexture =
    [](std::optional<const std::string> uri) -> std::shared_ptr<rendering::Texture> {
    return uri ? std::make_shared<rendering::Texture>(uri.value()) : nullptr;
};

struct Model
{
  public:
    Model(const std::string& uri,
          const std::optional<TextureLoaderFn> texture_loader_fn = DontLoadTexture);
    Model(Model&& other) = default;

    Model(Model&) = delete;
    Model(const Model&) = delete;
    Model& operator=(Model&) = delete;
    Model& operator=(const Model&) = delete;

    ~Model();

    std::vector<Mesh> meshes;

    std::string uri;
    Eigen::AlignedBox3f aabb;
    std::vector<std::string> texture_names;
};
}  // namespace rendering
