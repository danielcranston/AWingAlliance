#pragma once

#include <vector>
#include <string>
#include <array>

#include <functional>
#include <optional>

#include "resources/mesh_data.h"

namespace rendering
{
class Mesh
{
  public:
    Mesh(const resources::MeshData& data);
    Mesh(const std::string& name,
         const std::vector<float>& vertices,
         const std::vector<float>& normals,
         const std::vector<float>& texture_coords,
         const std::vector<uint>& indices,
         const std::array<float, 3>& min,
         const std::array<float, 3>& max,
         const std::string& diffuse_texname);

    ~Mesh();

    // Disallow copy constructor and copy assignment
    Mesh(Mesh&) = delete;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Move constructor/assignment operators that correctly transfers ownership to avoid
    // premature deallocation. Relevant:
    // https://www.khronos.org/opengl/wiki/Common_Mistakes#RAII_and_hidden_destructor_calls
    Mesh(Mesh&& other);
    Mesh& operator=(Mesh&& other);

    // Also allow const versions, possible thanks to is_owning being mutable. Relevant:
    // https://www.reddit.com/r/cpp/comments/8wbeom/coding_guideline_avoid_const_member_variables/
    Mesh(const Mesh&& other);
    Mesh& operator=(const Mesh&& other);

    const std::string& get_name() const;
    const int get_num_vertices() const;
    const int get_num_indices() const;
    const bool has_texture() const;
    const std::string& get_texture_name() const;
    const std::array<float, 3> get_min() const;
    const std::array<float, 3> get_max() const;
    const uint get_vao() const;

  private:
    std::string name;
    int num_vertices;
    int num_indices;
    std::string diffuse_texname;
    std::array<float, 3> min;
    std::array<float, 3> max;

    // Specifies ownership over the VBOs and VAO, i.e. if the objects will be cleaned up in the
    // destructor. Move constructors/assignment operators set this to false in the moved-from
    // object, so that ownership is passed correctly and no premature cleanup occurs.
    mutable bool is_owning = true;

    uint vao = 0;
    uint vbo_vertices = 0;
    uint vbo_indices = 0;
    uint vbo_normals = 0;
    uint vbo_texture_coords = 0;
};
}  // namespace rendering
