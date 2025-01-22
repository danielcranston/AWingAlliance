#include "rendering/render.h"
#include "rendering/global.h"
#include <iostream>
#include <cmath>
#include <map>

#include <GL/glew.h>

namespace rendering
{
namespace global
{
extern std::unique_ptr<Mesh> QUAD_MESH;
extern std::unique_ptr<Mesh> CUBE_MESH;

extern std::unique_ptr<ShaderProgram> MODEL_SHADER;
extern std::unique_ptr<ShaderProgram> SKYBOX_SHADER;
extern std::unique_ptr<ShaderProgram> SPARK_SHADER;
extern std::unique_ptr<ShaderProgram> SCREENSPACE_SHADER;

extern std::map<std::string, std::unique_ptr<ShaderProgram>> CUSTOM_SHADERS;
}  // namespace global

namespace
{
void render_mesh(const Mesh& mesh,
                 const ShaderProgram& shader_program,
                 const Eigen::Isometry3f& pose = Eigen::Isometry3f::Identity(),
                 const std::optional<RenderOptions>& options = std::nullopt,
                 const Texture* texture_override = nullptr)
{
    global::set_model_pose(pose.matrix());

    const ShaderProgram* program = &shader_program;
    if (options)
    {
        global::set_model_scale(options->scale ? options->scale.value() : Eigen::Vector3f::Ones());
        global::set_model_color((options->color && !mesh.texture) ? options->color.value() :
                                                                    Eigen::Vector3f::Ones());
        global::set_model_alpha(options->alpha ? options->alpha.value() : 1.0f);
        program = options->custom_shader_uri ?
                      global::CUSTOM_SHADERS.at(options->custom_shader_uri.value()).get() :
                      &shader_program;
    }

    const Texture* texture = mesh.texture ? mesh.texture.get() : nullptr;
    if (texture_override)
    {
        texture = texture_override;
    }

    if (texture)
    {
        global::use_texture(true);
        if (texture->type == Texture::Type::TEXTURE)
        {
            glBindTexture(GL_TEXTURE_2D, texture->texture_id);
        }
        else if (texture->type == Texture::Type::TEXTURE)
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture->texture_id);
        }
        else if (texture->type == Texture::Type::TEXTURE_ARRAY)
        {
            global::set_effect_num_layers(texture->num_layers);
            glBindTexture(GL_TEXTURE_2D_ARRAY, texture->texture_id);
        }
        else
        {
            throw std::runtime_error("Unexpected Texture::Type");
        }
    }
    else
    {
        global::use_texture(false);
    }

    global::use_program(*program);
    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, (const void*)0);
}
}  // namespace

void render_quad(const Texture& texture,
                 const Eigen::Isometry3f& pose,
                 const std::optional<RenderOptions>& options)
{
    global::cull_back_faces(false);
    global::use_alpha(true);

    render_mesh(*global::QUAD_MESH, *global::MODEL_SHADER, pose, options, &texture);

    global::use_alpha(false);
    global::cull_back_faces(true);
}

void render_model(const Model& model,
                  const Eigen::Isometry3f& pose,
                  const std::optional<RenderOptions>& options)
{
    global::use_alpha(true);
    for (const auto& mesh : model.meshes)
    {
        render_mesh(mesh, *global::MODEL_SHADER, pose, options);
    }
    global::use_alpha(false);
}

void render_skybox(const Texture& texture)
{
    global::write_depth_buffer(false);
    global::cull_back_faces(false);

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.texture_id);
    render_mesh(*global::CUBE_MESH, *global::SKYBOX_SHADER);

    global::write_depth_buffer(true);
    global::cull_back_faces(true);
}

void render_spark(const Eigen::Isometry3f& pose,
                  const float start_time,
                  const std::optional<RenderOptions>& options)
{
    global::use_alpha(true);
    global::cull_back_faces(false);

    global::set_effect_start_time(start_time);

    render_mesh(*global::QUAD_MESH, *global::SPARK_SHADER, pose, options);

    global::cull_back_faces(true);
    global::use_alpha(false);
}

void render_screen_transition(const float start_time, const std::optional<RenderOptions>& options)
{
    global::use_alpha(true);
    global::test_depth_buffer(false);

    global::set_effect_start_time(start_time);

    render_mesh(*global::QUAD_MESH,
                *global::SCREENSPACE_SHADER,
                Eigen::Isometry3f(Eigen::AngleAxisf(-M_PI / 2, Eigen::Vector3f::UnitZ()) *
                                  Eigen::AngleAxisf(M_PI / 2, Eigen::Vector3f::UnitY())),
                options);

    global::test_depth_buffer(true);
    global::use_alpha(false);
}
}  // namespace rendering