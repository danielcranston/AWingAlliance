#include <iostream>
#include "rendering/model.h"
#include "rendering/texture.h"
#include "rendering/render.h"
#include "rendering/shader_program.h"
#include "rendering/context_manager.h"

#include <GL/glew.h>

Eigen::Isometry3f make_pose(const Eigen::Vector3f& pos,
                            const Eigen::Quaternionf& quat = Eigen::Quaternionf::Identity())
{
    auto pose = Eigen::Isometry3f(quat);
    pose.translation() = pos;
    return pose;
}

Eigen::Matrix4f scale_matrix(const Eigen::Vector3f& scale)
{
    return scale.homogeneous().asDiagonal().toDenseMatrix();
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

    auto shader_program = rendering::ShaderProgram("model", "model.vert", "model.frag");
    auto skybox_program = rendering::ShaderProgram("skybox", "sky.vert", "sky.frag");

    auto ship_model = rendering::Model("awing.obj", rendering::AutoLoadTexture);

    auto skybox_model = rendering::Model("cube.obj", [](const std::string&) {
        return std::make_shared<rendering::Texture>("skybox/new",
                                                    rendering::Texture::Type::CUBEMAP);
    });

    auto camera_pose = Eigen::Isometry3f::Identity();
    auto model_pose = make_pose({ 0.0f, 0.0f, -16.0f });

    bool should_shutdown = false;
    SDL_Event event;
    while (!should_shutdown)
    {
        // Start throwaway
        // TODO: Wrap rendering of models/meshes in a nice way
        // TODO: Use Uniform Buffer Objects: https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL

        SDL_SetRelativeMouseMode(SDL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(true);
        glDisable(GL_BLEND);

        shader_program.use();
        shader_program.setUniform3fv("uniform_color", Eigen::Vector3f(1.0, 0.0, 0.0));
        shader_program.setUniformMatrix4fv("perspective",
                                           perspective(M_PI / 180.0f * 90.0,  //
                                                       1200.0 / 900.0,
                                                       5.0,       // 5.0,
                                                       8192.0));  // 8192.0));
        shader_program.setUniformMatrix4fv("camera", camera_pose.matrix());
        shader_program.setUniformMatrix4fv("model_scale", scale_matrix({ 1.0f, 1.0f, 1.0f }));

        rendering::render(ship_model, shader_program, model_pose);

        SDL_GL_SwapWindow(context_manager.window);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
            {
                should_shutdown = true;
            }
            if (event.type == SDL_MOUSEMOTION)
            {
                model_pose = model_pose *
                             Eigen::AngleAxisf(0.01 * event.motion.xrel, Eigen::Vector3f::UnitY()) *
                             Eigen::AngleAxisf(0.01 * event.motion.yrel, Eigen::Vector3f::UnitX());

                // std::cout << event.motion.x << " " << event.motion.y << " (relative "
                //           << event.motion.xrel << " " << event.motion.yrel << ")" << std::endl;
            }
        }
    }

    std::cout << "Shutdown" << std::endl;
}