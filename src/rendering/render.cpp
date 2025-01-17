#include "rendering/render.h"
#include <iostream>

#include <GL/glew.h>

namespace rendering
{
namespace
{
void render_mesh(const Mesh& mesh,
                 const ShaderProgram& shader_program,
                 const Eigen::Isometry3f& pose)
{
    shader_program.set_uniform("model_pose", pose.matrix());

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

    for (const auto& mesh : model.get_meshes())
    {
        render_mesh(mesh, shader_program, pose);
    }
}

// void render_billboard(const ShaderProgram& shader_program,
//                       const Eigen::Isometry3f& pose,
//                       const Eigen::Vector3f& scale)
// {
//     shader_program.use();
//     shader_spark.set_uniform("model_scale", billboard_component.size);
//     shader_spark.setUniform1f("start_time", billboard_component.birth_time);
//     rendering::draw_colored(shader_spark,
//                             quad_mesh,
//                             motion_state.pose(),
//                             Eigen::Vector3f(0.0f, 0.0f, 1.0f),
//                             GL_TRIANGLES);
// }
}  // namespace rendering