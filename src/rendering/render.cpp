#include "rendering/render.h"
#include "rendering/global.h"
#include <iostream>
#include <cmath>

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
}  // namespace global

namespace
{
void render_mesh(const Mesh& mesh,
                 const ShaderProgram& shader_program,
                 const Eigen::Isometry3f& pose = Eigen::Isometry3f::Identity())
{
    global::set_model_pose(pose.matrix());

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

void render_model(const Model& model, const Eigen::Isometry3f& pose)
{
    global::use_program(*global::MODEL_SHADER);

    for (const auto& mesh : model.meshes)
    {
        render_mesh(mesh, *global::MODEL_SHADER, pose);
    }
}

void render_skybox(const Texture& texture)
{
    global::use_program(*global::SKYBOX_SHADER);

    global::write_depth_buffer(false);
    global::cull_back_faces(false);

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.texture_id);
    render_mesh(*global::CUBE_MESH, *global::SKYBOX_SHADER);

    global::write_depth_buffer(true);
    global::cull_back_faces(true);
}

void render_spark(const Eigen::Isometry3f& pose, const Eigen::Vector3f& scale)
{
    global::use_alpha(true);
    global::cull_back_faces(false);
    global::use_program(*global::SPARK_SHADER);

    global::set_model_scale(scale);
    render_mesh(*global::QUAD_MESH, *global::SPARK_SHADER, pose);

    global::cull_back_faces(true);
    global::use_alpha(false);
}

void render_screen_transition()
{
    global::use_alpha(true);
    global::test_depth_buffer(false);
    global::use_program(*global::SCREENSPACE_SHADER);

    global::set_model_scale(2 * Eigen::Vector3f::Ones());
    render_mesh(*global::QUAD_MESH,
                *global::SPARK_SHADER,
                Eigen::Isometry3f(Eigen::AngleAxisf(-M_PI / 2, Eigen::Vector3f::UnitZ()) *
                                  Eigen::AngleAxisf(M_PI / 2, Eigen::Vector3f::UnitY())));

    global::test_depth_buffer(true);
    global::use_alpha(false);
}
}  // namespace rendering