#include <string>
#include <iostream>

#include <GL/glew.h>

#include "rendering/mesh.h"

namespace rendering
{
Mesh::Mesh(const resources::MeshData& data)
  : Mesh(data.name,
         data.vertices,
         data.normals,
         data.texture_coords,
         data.indices,
         data.min,
         data.max,
         data.diffuse_texname)
{
}

Mesh::Mesh(const std::string& name,
           const std::vector<float>& vertices,
           const std::vector<float>& normals,
           const std::vector<float>& texture_coords,
           const std::vector<uint>& indices,
           const std::array<float, 3>& min,
           const std::array<float, 3>& max,
           const std::string& diffuse_texname)
  : name(name),
    num_vertices(vertices.size() / 3),
    num_indices(indices.size()),
    diffuse_texname(diffuse_texname),
    min(min),
    max(max)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo_vertices);
    glGenBuffers(1, &vbo_normals);
    glGenBuffers(1, &vbo_texture_coords);
    glGenBuffers(1, &vbo_indices);

    // std::cout << "Mesh with texture \"" << diffuse_texname << "\" (vao id " << vao
    //           << ") being constructed" << std::endl;

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_coords);
    glBufferData(GL_ARRAY_BUFFER,
                 texture_coords.size() * sizeof(float),
                 texture_coords.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int),
                 indices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Mesh::Mesh(Mesh&& other)
  : name(std::move(other.name)),
    num_vertices(other.num_vertices),
    num_indices(other.num_indices),
    diffuse_texname(other.diffuse_texname),
    min(other.min),
    max(other.max),
    vao(other.vao),
    vbo_vertices(other.vbo_vertices),
    vbo_indices(other.vbo_indices),
    vbo_normals(other.vbo_normals),
    vbo_texture_coords(other.vbo_texture_coords)
{
    // std::cout << "Mesh \"" << name << "\" (vao id " << vao << ") being moved" << std::endl;

    // End others ownership of the mesh, preventing it from releasing the vao/vbos in its
    // destructor.
    other.is_owning = false;
}

Mesh::Mesh(const Mesh&& other)
  : name(std::move(other.name)),
    num_vertices(other.num_vertices),
    num_indices(other.num_indices),
    diffuse_texname(other.diffuse_texname),
    min(other.min),
    max(other.max),
    vao(other.vao),
    vbo_vertices(other.vbo_vertices),
    vbo_indices(other.vbo_indices),
    vbo_normals(other.vbo_normals),
    vbo_texture_coords(other.vbo_texture_coords)
{
    // std::cout << "Mesh \"" << name << "\" (vao id " << vao << ") being moved from const"
    //           << std::endl;

    // End others ownership of the mesh, preventing it from releasing the vao/vbos in its
    // destructor.
    other.is_owning = false;
}

Mesh& Mesh::operator=(Mesh&& other)
{
    // std::cout << "Mesh \"" << other.name << "\" (vao id " << other.vao
    //           << ") move assignment operator" << std::endl;

    name = other.name;
    num_vertices = other.num_vertices;
    num_indices = other.num_indices;
    diffuse_texname = other.diffuse_texname;
    min = other.min;
    max = other.max;

    vao = other.vao;
    vbo_vertices = other.vbo_vertices;
    vbo_indices = other.vbo_indices;
    vbo_normals = other.vbo_normals;
    vbo_texture_coords = other.vbo_texture_coords;

    // End others ownership of the mesh, preventing it from releasing the vao/vbos in its
    // destructor.
    other.is_owning = false;

    return *this;
}

Mesh& Mesh::operator=(const Mesh&& other)
{
    // std::cout << "Mesh \"" << other.name << "\" (vao id " << other.vao
    //           << ") const move assignment operator" << std::endl;

    name = other.name;
    num_vertices = other.num_vertices;
    num_indices = other.num_indices;
    diffuse_texname = other.diffuse_texname;
    min = other.min;
    max = other.max;

    vao = other.vao;
    vbo_vertices = other.vbo_vertices;
    vbo_indices = other.vbo_indices;
    vbo_normals = other.vbo_normals;
    vbo_texture_coords = other.vbo_texture_coords;

    // End others ownership of the mesh, preventing it from releasing the vao/vbos in its
    // destructor.
    other.is_owning = false;

    return *this;
}

Mesh::~Mesh()
{
    if (is_owning)
    {
        // std::cout << "Mesh \"" << name << "\" (vao id " << vao << ") being cleaned up" <<
        // std::endl;
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo_vertices);
        glDeleteBuffers(1, &vbo_indices);
        glDeleteBuffers(1, &vbo_normals);
        glDeleteBuffers(1, &vbo_texture_coords);
    }
    else
    {
        // std::cout << "  (Moved-from Mesh is safely discarded without freeing any resources)"
        //           << std::endl;
    }
}

const std::string& Mesh::get_name() const
{
    return name;
};

const int Mesh::get_num_vertices() const
{
    return num_vertices;
};

const int Mesh::get_num_indices() const
{
    return num_indices;
};

const std::string& Mesh::get_texture_name() const
{
    return diffuse_texname;
};

const bool Mesh::has_texture() const
{
    return diffuse_texname != "";
}

const std::array<float, 3> Mesh::get_min() const
{
    return min;
}

const std::array<float, 3> Mesh::get_max() const
{
    return max;
}

const uint Mesh::get_vao() const
{
    return vao;
};

}  // namespace rendering
