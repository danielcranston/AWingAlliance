#include <iostream>

#include <GL/glew.h>

#include "rendering/draw.h"

namespace rendering
{
void draw(const ShaderProgram& program,
          const Mesh& mesh,
          const Eigen::Isometry3f& pose,
          const int mode)
{
    program.setUniformMatrix4fv("model_pose", pose.matrix());

    if (mode != GL_TRIANGLES && mode != GL_LINES)
        throw std::runtime_error("Invalid draw mode " + std::to_string(mode));

    glBindVertexArray(mesh.get_vao());
    glDrawElements(mode, mesh.get_num_indices(), GL_UNSIGNED_INT, (const void*)0);
}

void draw_textured(const ShaderProgram& program,
                   const Mesh& mesh,
                   const Eigen::Isometry3f& pose,
                   const Texture& texture,
                   const int mode)
{
    program.setUniform1i("use_color", 0);
    if (texture.type == Texture::Type::TEXTURE)
    {
        glBindTexture(GL_TEXTURE_2D, texture.texture_id);
    }
    else if (texture.type == Texture::Type::CUBEMAP)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture.texture_id);
    }
    else
    {
        throw std::runtime_error("Unexpected texture type: " +
                                 std::to_string(static_cast<int>(texture.type)));
    }

    draw(program, mesh, pose, mode);
}

void draw_colored(const ShaderProgram& program,
                  const Mesh& mesh,
                  const Eigen::Isometry3f& pose,
                  const Eigen::Vector3f& color,
                  const int mode)
{
    program.setUniform1i("use_color", 1);
    program.setUniform3fv("uniform_color", color);

    draw(program, mesh, pose, mode);
}

}  // namespace rendering
