#include <iostream>
#include "rendering/model.h"
#include "rendering/texture.h"
#include "rendering/render.h"
#include "rendering/shader_program.h"
#include "rendering/context_manager.h"
#include "rendering/global.h"

#include <GL/glew.h>

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

    rendering::global::set_camera_perspective(perspective(M_PI / 180.0f * 45.0,  //
                                                          1200.0 / 900.0,
                                                          1,
                                                          8192.0));
    auto model_pose = Eigen::Isometry3f::Identity();
    auto T_model_spark = make_pose({ 0.0f, 0.0f, 5.0f });
    auto camera_pose = make_pose({ 0.0f, 0.0f, -50.0f });
    int camera_rotate_dir = 0;
    auto spark_scale = Eigen::Vector3f::Ones() * 15.0f;

    bool should_shutdown = false;
    while (!should_shutdown)
    {
        rendering::global::clear_frame(true, true);

        rendering::global::set_camera_pose(camera_pose.matrix());

        rendering::render_skybox(sky_texture);

        float time = SDL_GetTicks() / 1000.0f;
        rendering::global::set_model_scale(Eigen::Vector3f::Ones() *
                                           (0.1f * std::sin(2 * time) + 1.0f));

        model_pose.translation().x() = 5.0f * std::sin(time);
        rendering::render_model(ship_model, model_pose);

        rendering::global::set_effect_current_time(time);

        float start_time = std::floor(time / 1.0f) * 1.0f;

        rendering::render_spark(model_pose * T_model_spark, spark_scale, start_time);
        rendering::render_screen_transition();

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