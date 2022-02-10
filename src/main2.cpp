#include <iostream>
#include <vector>
#include <list>
#include <map>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <GL/glew.h>

#include <SDL2/SDL.h>

#include <entt/entt.hpp>

#include "resources/load_model.h"
#include "resources/load_texture.h"
#include "resources/load_geometry.h"
#include "rendering/model.h"
#include "rendering/mesh.h"
#include "rendering/compile_shader_program.h"
#include "rendering/primitives.h"
#include "rendering/context_manager.h"
#include "rendering/draw.h"
#include "geometry/geometry.h"
#include "geometry/collision.h"
#include "control/camera_controller.h"
#include "convenience.h"
#include "environment/environment.h"
#include "scene.h"

// Move to src/ecs/systems
void draw2(const rendering::ShaderProgram& program,
           const rendering::Model& model,
           const Eigen::Isometry3f& pose,
           const Eigen::Vector3f& color,
           const entt::resource_cache<rendering::Texture>& texture_cache,
           const int mode)
{
    program.use();
    program.setUniformMatrix4fv("model_pose", pose.matrix());
    for (const auto& mesh : model.get_meshes())
    {
        if (mesh.has_texture())
        {
            program.setUniform1i("use_color", 0);

            auto texture_uri = entt::hashed_string(mesh.get_texture_name().c_str());

            if (!texture_cache.contains(texture_uri))
            {
                throw std::runtime_error("draw: texture not found in texture cache: " +
                                         mesh.get_texture_name());
            }

            const rendering::Texture& tex = *texture_cache.handle(texture_uri);
            if (tex.type == rendering::Texture::Type::TEXTURE)
            {
                glBindTexture(GL_TEXTURE_2D, tex.texture_id);
            }
            else if (tex.type == rendering::Texture::Type::CUBEMAP)
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
            program.setUniform3fv("uniform_color", color);
        }

        if (mode != GL_TRIANGLES && mode != GL_LINES)
            throw std::runtime_error("Invalid draw mode " + std::to_string(mode));

        glBindVertexArray(mesh.get_vao());
        glDrawElements(mode, mesh.get_num_indices(), GL_UNSIGNED_INT, (const void*)0);
    }
}

int main(int argc, char* argv[])
{
    int screen_w = 1200;
    int screen_h = 900;

    convenience::init_sdl();
    rendering::ContextManager context_manager{ "Main Window", screen_w, screen_h };
    convenience::init_glew(screen_w, screen_h);

    auto scene = Scene::load_from_scenario("scenario");

    float current_time = SDL_GetTicks() / 1000.0f;
    const float dt = 1.0f / 60.f;
    float t = 0.0f;
    float accumulator = 0.0f;

    bool should_shutdown = false;
    while (!should_shutdown)
    {
        float new_time = SDL_GetTicks() / 1000.0f;
        float frameTime = new_time - current_time;
        current_time = new_time;

        accumulator += frameTime;

        while (accumulator >= dt)
        {
            accumulator -= dt;
            t += dt;
        }

        auto& shader_skybox = scene->shader_cache.handle(entt::hashed_string("skybox")).get();
        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        for (const auto [entity, skybox_component] : scene->registry.view<SkyboxComponent>().each())
        {
            shader_skybox.use();
            shader_skybox.setUniformMatrix4fv("camera", Eigen::Matrix4f::Identity());
            rendering::draw(shader_skybox,
                            *scene->model_cache.handle(skybox_component.model_uri),
                            Eigen::Isometry3f::Identity(),
                            { 1.0f, 0.0f, 0.0f },
                            *scene->texture_cache.handle(skybox_component.texture_uri),
                            GL_TRIANGLES);
        }
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Draw Actors and derived classes
        auto& shader_model = scene->shader_cache.handle(entt::hashed_string("model")).get();
        shader_model.use();
        Eigen::Isometry3f temp =
            geometry::make_pose(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Quaternionf::Identity());
        shader_model.setUniformMatrix4fv("camera", temp.matrix());
        auto view = scene->registry.view<geometry::MotionState, VisualComponent>();
        for (const auto [entity, motion_state, visual_component] : view.each())
        {
            draw2(shader_model,
                  *scene->model_cache.handle(visual_component.texture_uri),
                  geometry::make_pose(motion_state.position, motion_state.orientation),
                  { 1.0f, 0.0f, 0.0f },
                  scene->texture_cache,
                  GL_TRIANGLES);
        }

        SDL_GL_SwapWindow(context_manager.window);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
            {
                should_shutdown = true;
            }
        }
    }
}
