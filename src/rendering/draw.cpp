#include <iostream>

#include <GL/glew.h>

#include "rendering/draw.h"

namespace rendering
{
void draw(const ShaderProgram& program,
          const Model& model,
          const Eigen::Isometry3f& pose,
          const Eigen::Vector3f& color,
          const std::map<std::string, rendering::Texture>& textures_map,
          const int mode)
{
    program.use();
    program.setUniformMatrix4fv("model_pose", pose.matrix());
    for (const auto& mesh : model.get_meshes())
    {
        if (mesh.has_texture())
        {
            program.setUniform1i("use_color", 0);

            if (textures_map.find(mesh.get_texture_name()) == textures_map.end())
            {
                throw std::runtime_error("draw: texture not found in textures map: " +
                                         mesh.get_texture_name());
            }

            const Texture& tex = textures_map.at(mesh.get_texture_name());
            if (tex.type == Texture::Type::TEXTURE)
            {
                glBindTexture(GL_TEXTURE_2D, tex.texture_id);
            }
            else if (tex.type == Texture::Type::CUBEMAP)
            {
                glBindTexture(GL_TEXTURE_CUBE_MAP, tex.texture_id);
            }
            else
            {
                throw std::runtime_error("Unexpected texture type: " +
                                         std::to_string(static_cast<int>(tex.type)));
            }
        }
        else
        {
            program.setUniform1i("use_color", 1);
        }

        if (mode != GL_TRIANGLES && mode != GL_LINES)
            throw std::runtime_error("Invalid draw mode " + std::to_string(mode));

        glBindVertexArray(mesh.get_vao());
        // glDrawElements(GL_TRIANGLES, mesh.get_num_indices(), GL_UNSIGNED_INT, (const void*)0);
        glDrawElements(mode, mesh.get_num_indices(), GL_UNSIGNED_INT, (const void*)0);
    }
}

void draw(const ShaderProgram& program,
          const Model& model,
          const Eigen::Isometry3f& pose,
          const Eigen::Vector3f& color,
          const std::optional<std::reference_wrapper<const Texture>> texture,
          const int mode)
{
    program.use();
    program.setUniformMatrix4fv("model_pose", pose.matrix());
    for (const auto& mesh : model.get_meshes())
    {
        if (texture.has_value())
        {
            const Texture& tex = texture.value().get();
            program.setUniform1i("use_color", 0);
            if (tex.type == Texture::Type::TEXTURE)
            {
                glBindTexture(GL_TEXTURE_2D, tex.texture_id);
            }
            else if (tex.type == Texture::Type::CUBEMAP)
            {
                glBindTexture(GL_TEXTURE_CUBE_MAP, tex.texture_id);
            }
            else
            {
                throw std::runtime_error("Unexpected texture type: " +
                                         std::to_string(static_cast<int>(tex.type)));
            }
        }
        else
        {
            std::cout << "is not texture" << std::endl;
            program.setUniform1i("use_color", 1);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        if (mode != GL_TRIANGLES && mode != GL_LINES)
            throw std::runtime_error("Invalid draw mode " + std::to_string(mode));

        glBindVertexArray(mesh.get_vao());
        // glDrawElements(GL_TRIANGLES, mesh.get_num_indices(), GL_UNSIGNED_INT, (const void*)0);
        glDrawElements(mode, mesh.get_num_indices(), GL_UNSIGNED_INT, (const void*)0);
    }
}

}  // namespace rendering