#include <iostream>

#include <GL/glew.h>

#include "rendering/context_manager.h"
#include "rendering/global.h"
#include "rendering/model.h"
#include "rendering/render.h"
#include "rendering/shader_program.h"
#include "rendering/texture.h"

Eigen::Isometry3f make_pose(const Eigen::Vector3f& pos,
                            const Eigen::Quaternionf& quat = Eigen::Quaternionf::Identity())
{
    auto pose = Eigen::Isometry3f(quat);
    pose.translation() = pos;
    return pose;
}

// https://stackoverflow.com/questions/14971712/eigen-perspective-projection-matrix
Eigen::Matrix4f
perspective(const float fov_y, const float aspect, const float z_near, const float z_far)
{
    float y_scale = 1.0f / std::tan(fov_y / 2);
    float x_scale = y_scale / aspect;

    Eigen::Matrix4f res;
    res << x_scale, 0, 0, 0, 0, y_scale, 0, 0, 0, 0, -(z_far + z_near) / (z_far - z_near), -1, 0, 0,
        -2 * z_near * z_far / (z_far - z_near), 0;

    return res.transpose();
}

int main(int argc, char* argv[])
{
    std::cout << "start main" << std::endl;

    auto context_manager = rendering::ContextManager("Main Window", 1200, 900);

    auto ship_model = rendering::Model("awing.obj", rendering::AutoLoadTexture);

    auto sky_texture =
        rendering::Texture("skybox/lightblue/512", rendering::Texture::Type::CUBEMAP);
    auto sprite_texture_array = rendering::Texture("sprites/storm_trooper_walking_se",
                                                   rendering::Texture::Type::TEXTURE_ARRAY);

    rendering::global::register_custom_shaders(
        { { "hyperspace_tunnel", "screenspace.vert", "hyperspace_tunnel.frag" },
          { "hyperspace_jump", "screenspace.vert", "hyperspace_jump.frag" },
          { "sprite", "model.vert", "sprite.frag" },
          { "spark", "model.vert", "spark.frag" } });

    rendering::global::set_camera_perspective(perspective(M_PI / 180.0f * 45.0,  //
                                                          1200.0 / 900.0,
                                                          1,
                                                          8192.0));
    auto model_pose = Eigen::Isometry3f::Identity();
    auto T_model_spark = make_pose({ 0.0f, 0.0f, 5.0f });
    auto camera_pose = make_pose({ 0.0f, 0.0f, -50.0f });
    int camera_rotate_dir = 0;
    auto model_options = rendering::RenderOptions();
    model_options.scale = Eigen::Vector3f::Ones();
    model_options.color = Eigen::Vector3f(0.5f, 0.0f, 0.0f);
    auto effect_options = rendering::RenderOptions();

    bool should_shutdown = false;
    while (!should_shutdown)
    {
        // Update

        float time = SDL_GetTicks() / 1000.0f;
        float effect_start_time = std::floor(time / 1.0f) * 1.0f;

        model_pose.translation().x() = 5.0f * std::sin(time);
        model_options.scale->z() = 0.5 * std::sin(2 * time) + 1.0f;
        model_options.alpha = model_options.scale->z();

        // Render

        rendering::global::clear_frame(true, true);

        rendering::global::set_camera_pose(camera_pose.matrix());

        rendering::render_skybox(sky_texture);

        rendering::render_model(ship_model, model_pose, model_options);

        rendering::global::set_effect_current_time(time);

        switch (static_cast<int>(effect_start_time) % 8)
        {
            case 0:
                effect_options.scale = 15 * Eigen::Vector3f::Ones();
                effect_options.custom_shader_uri = "spark";
                effect_options.custom_texture = nullptr;
                rendering::global::set_effect_start_time(effect_start_time);
                rendering::render_quad(model_pose * T_model_spark, effect_options);
                break;
            case 1:
                effect_options.custom_shader_uri = std::nullopt;
                effect_options.scale = 2 * Eigen::Vector3f::Ones();
                rendering::render_fullscreen(effect_start_time, effect_options);
                break;
            case 2:
                effect_options.custom_shader_uri = "hyperspace_tunnel";
                rendering::render_fullscreen(effect_start_time, effect_options);
                break;
            case 3:
                effect_options.custom_shader_uri = "hyperspace_jump";
                rendering::render_fullscreen(effect_start_time, effect_options);
                break;
            default:
                effect_options.scale = 10 * Eigen::Vector3f::Ones();
                effect_options.custom_shader_uri = "sprite";
                effect_options.custom_texture = &sprite_texture_array;
                rendering::render_quad(model_pose * T_model_spark, effect_options);
                break;
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
            else if (event.type == SDL_MOUSEMOTION)
            {
                model_pose =
                    model_pose *
                    Eigen::AngleAxisf(-0.005 * event.motion.xrel, Eigen::Vector3f::UnitZ()) *
                    Eigen::AngleAxisf(-0.005 * event.motion.yrel, Eigen::Vector3f::UnitY());
            }
            else if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && !event.key.repeat)
            {
                if (event.key.keysym.sym == SDLK_a)
                {
                    camera_rotate_dir += event.type == SDL_KEYDOWN ? 1 : -1;
                }
                else if (event.key.keysym.sym == SDLK_d)
                {
                    camera_rotate_dir -= event.type == SDL_KEYDOWN ? 1 : -1;
                }
            }
        }

        if (camera_rotate_dir != 0)
        {
            camera_pose =
                camera_pose * Eigen::AngleAxisf(0.01 * camera_rotate_dir, Eigen::Vector3f::UnitY());
        }
    }

    std::cout << "Shutdown" << std::endl;
}
