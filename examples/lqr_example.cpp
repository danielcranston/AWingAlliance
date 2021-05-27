#include <iostream>
#include <vector>
#include <list>
#include <map>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <GL/glew.h>

#include <SDL2/SDL.h>

#include "resources/load_model.h"
#include "resources/load_texture.h"
#include "resources/load_descriptions.h"
#include "rendering/model.h"
#include "rendering/mesh.h"
#include "rendering/compile_shader_program.h"
#include "rendering/primitives.h"
#include "rendering/context_manager.h"
#include "rendering/rendering_manager.h"
#include "rendering/draw.h"
#include "geometry.h"
#include "actor/ship.h"
#include "actor/camera.h"
#include "actor/laser.h"
#include "control/position_controller.h"
#include "control/orientation_controller.h"
#include "control/camera_controller.h"

using geometry::make_pose;

int main(int argc, char* argv[])
{
    int screen_w = 1200;
    int screen_h = 900;

    if (SDL_Init(SDL_INIT_EVERYTHING | SDL_VIDEO_OPENGL) != 0)
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    const auto context_flags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | SDL_GL_CONTEXT_DEBUG_FLAG;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, context_flags);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    rendering::ContextManager context_manager{ "LQR Example", screen_w, screen_h };

    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK)
    {
        std::stringstream ss;
        ss << "Error initializing GLEW: " << glewGetErrorString(glew_status);
        throw std::runtime_error(ss.str());
    }
    else
        std::cout << "GLEW Initialized: " << glewGetErrorString(glew_status) << std::endl;

    glViewport(0, 0, screen_w, screen_h);
    glClearColor(0, 0, 0, 1);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    SDL_ShowCursor(false);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    float FOV_Y = 60.0f * M_PI / 180.0f;
    float aspect = 1.0f * screen_w / screen_h;
    const auto perspective = geometry::perspective(FOV_Y, aspect, 5.0f, 8192.0f);

    auto desc = resources::load_descriptions();

    // ACTORS
    auto awing =
        actor::Ship("awing", desc.at("awing"), { 0.0f, -5.0f, -30.0f }, { 0.0f, 0.0f, 0.0f, 1.0f });
    auto camera =
        actor::Camera("camera", { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 0.0f }, perspective);

    // RENDERING MANAGER
    auto rendering_manager = rendering::RenderingManager();

    rendering_manager.register_shader_program(
        "model", "model.vert", "model.frag", [&perspective](rendering::ShaderProgram* program) {
            program->use();
            program->setUniform1i("tex", 0);
            program->setUniformMatrix4fv("perspective", perspective);
            program->setUniformMatrix4fv("model_scale", Eigen::Matrix4f::Identity());
        });
    auto& shader_model = rendering_manager.get_shader_program("model");

    rendering_manager.register_shader_program(
        "skybox",
        "sky.vert",
        "sky.frag",
        [FOV_Y, aspect, &camera](rendering::ShaderProgram* program) {
            program->use();
            program->setUniform1i("tex", 0);
            program->setUniformMatrix4fv("perspective",
                                         geometry::perspective(FOV_Y, aspect, 0.5f, 8192.0f));
        });
    auto& shader_skybox = rendering_manager.get_shader_program("skybox");

    const std::string skybox_tex = "skybox/lightblue/512";
    rendering_manager.register_skybox(skybox_tex, resources::load_cubemap_texture(skybox_tex));

    rendering_manager.register_models({ awing.get_visual_name() }, [](const std::string& filename) {
        return resources::load_model(filename);
    });

    rendering_manager.register_unloaded_textures(
        [](const std::string& filename) { return resources::load_texture(filename); });

    // CONTROL RELATED
    auto controller = control::PositionController(awing.get_position());

    auto ori_controller = control::OrientationController(awing.get_orientation());

    auto camera_controller =
        control::CameraController(awing.get_position(), awing.get_orientation());
    camera.set_tick_behavior([&awing, &ori_controller, &camera_controller]() {
        camera_controller.set_target_pose(awing.get_pose() * make_pose({ 0.0f, 5.0f, 15.0f }));
        camera_controller.update(0.0f, 1.0f / 60.0f);

        return camera_controller.get_pose();
    });

    // BEGIN LOOP
    SDL_Event event;
    bool should_shutdown = false;

    float current_time = SDL_GetTicks() / 1000.0f;
    float dt = 1.0f / 60.f;
    float t = 0.0f;
    float accumulator = 0.0f;

    while (!should_shutdown)
    {
        float new_time = SDL_GetTicks() / 1000.0f;
        float frameTime = new_time - current_time;
        current_time = new_time;

        accumulator += frameTime;

        // Integrate
        while (accumulator >= dt)
        {
            controller.update(dt);
            ori_controller.update(dt);

            awing.set_pose(
                make_pose(controller.get_position(), ori_controller.get_state_quaternion()));
            camera.tick(t, dt);

            accumulator -= dt;
            t += dt;
        }

        // Render
        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        shader_skybox.use();
        shader_skybox.setUniformMatrix4fv("camera", camera.get_pose().matrix().inverse());
        rendering::draw(shader_skybox,
                        rendering_manager.get_model("box"),
                        Eigen::Isometry3f(camera.get_pose().linear()),
                        { 1.0f, 0.0f, 0.0f },
                        rendering_manager.get_texture(skybox_tex),
                        GL_TRIANGLES);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        shader_model.use();
        shader_model.setUniformMatrix4fv("camera", camera.get_pose().matrix().inverse());
        glDisable(GL_BLEND);
        rendering::draw(shader_model,
                        rendering_manager.get_model(awing.get_visual_name()),
                        awing.get_pose(),
                        { 1.0f, 0.0f, 0.0f },
                        rendering_manager.get_textures(),
                        GL_TRIANGLES);
        glEnable(GL_BLEND);

        rendering::draw(
            shader_model,
            rendering_manager.get_model(awing.get_visual_name()),
            make_pose(controller.get_goal_position(), ori_controller.get_goal_quaternion()),
            { 1.0f, 0.0f, 0.0f },
            rendering_manager.get_textures(),
            GL_TRIANGLES);

        SDL_GL_SwapWindow(context_manager.window);

        controller.set_goal_position(awing.get_position() + 10.0f * awing.get_fwd_dir());

        // Process input
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                should_shutdown = true;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    should_shutdown = true;
                }
                else if (!event.key.repeat && event.key.keysym.sym == SDLK_w)
                {
                    controller.update_goal_position(+10.0f * awing.get_fwd_dir());
                }
                else if (!event.key.repeat && event.key.keysym.sym == SDLK_s)
                {
                    controller.update_goal_position(-10.0f * awing.get_fwd_dir());
                }
                else if (!event.key.repeat && event.key.keysym.sym == SDLK_a)
                {
                    controller.update_goal_position(-10.0f * awing.get_right_dir());
                }
                else if (!event.key.repeat && event.key.keysym.sym == SDLK_d)
                {
                    controller.update_goal_position(+10.0f * awing.get_right_dir());
                }
                else if (!event.key.repeat && event.key.keysym.sym == SDLK_LCTRL)
                {
                    controller.update_goal_position(-10.0f * awing.get_up_dir());
                }
                else if (!event.key.repeat && event.key.keysym.sym == SDLK_SPACE)
                {
                    controller.update_goal_position(+10.0f * awing.get_up_dir());
                }
                else if (!event.key.repeat && event.key.keysym.sym == SDLK_LEFT)
                {
                    ori_controller.update_goal_quaternion(Eigen::Quaternionf(
                        Eigen::AngleAxis<float>(M_PI / 4.0f, Eigen::Vector3f::UnitZ())));
                }
                else if (!event.key.repeat && event.key.keysym.sym == SDLK_RIGHT)
                {
                    ori_controller.update_goal_quaternion(Eigen::Quaternionf(
                        Eigen::AngleAxis<float>(-M_PI / 4.0f, Eigen::Vector3f::UnitZ())));
                }
                else if (!event.key.repeat && event.key.keysym.sym == SDLK_DOWN)
                {
                    ori_controller.update_goal_quaternion(Eigen::Quaternionf(
                        Eigen::AngleAxis<float>(M_PI / 4.0f, Eigen::Vector3f::UnitX())));
                }
                else if (!event.key.repeat && event.key.keysym.sym == SDLK_UP)
                {
                    ori_controller.update_goal_quaternion(Eigen::Quaternionf(
                        Eigen::AngleAxis<float>(-M_PI / 4.0f, Eigen::Vector3f::UnitX())));
                }
                else if (!event.key.repeat && event.key.keysym.sym == SDLK_r)
                {
                    ori_controller.set_goal_quaternion(Eigen::Quaternionf::Identity());
                }
            }
        }
    }
}