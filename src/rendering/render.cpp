#include "rendering/render.h"
#include "rendering/global.h"
#include <iostream>

#include <GL/glew.h>

namespace rendering
{
namespace global
{
extern std::unique_ptr<Mesh> QUAD_MESH;
extern std::unique_ptr<Mesh> CUBE_MESH;
}  // namespace global

namespace
{
void render_mesh(const Mesh& mesh,
                 const ShaderProgram& shader_program,
                 const Eigen::Isometry3f& pose)
{
    rendering::global::set_model_pose(pose.matrix());

    if (mesh.texture)
    {
        shader_program.set_uniform("use_color", 0);

        glBindTexture(mesh.texture->type == Texture::Type::TEXTURE ? GL_TEXTURE_2D :
                                                                     GL_TEXTURE_CUBE_MAP,
                      mesh.texture->texture_id);
    }
    else
    {
        shader_program.set_uniform("use_color", 1);
    }

    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, (const void*)0);
}
}  // namespace

void render(const Model& model, const ShaderProgram& shader_program, const Eigen::Isometry3f& pose)
{
    shader_program.use();

    for (const auto& mesh : model.meshes)
    {
        render_mesh(mesh, shader_program, pose);
    }
}

void render_billboard(const ShaderProgram& shader_program,
                      const Eigen::Isometry3f& pose,
                      const Eigen::Vector3f& scale)

{
    rendering::global::set_model_scale(scale);
    render_mesh(*global::QUAD_MESH, shader_program, pose);
}
}  // namespace rendering