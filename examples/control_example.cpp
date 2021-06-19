#include <iostream>
#include <vector>
#include <list>
#include <map>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <GL/glew.h>

#include <SDL2/SDL.h>

#include "convenience.h"
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

const std::unordered_map<int, control::MotionControl::States> key_mapping = {
    { SDLK_UP, control::MotionControl::States::TURN_UP },
    { SDLK_DOWN, control::MotionControl::States::TURN_DOWN },
    { SDLK_LEFT, control::MotionControl::States::ROLL_LEFT },
    { SDLK_RIGHT, control::MotionControl::States::ROLL_RIGHT },
    { SDLK_a, control::MotionControl::States::TURN_LEFT },
    { SDLK_d, control::MotionControl::States::TURN_RIGHT },
    { SDLK_w, control::MotionControl::States::ACC_INCREASE },
    { SDLK_s, control::MotionControl::States::ACC_DECREASE }
};

void update_input_request(SDL_Event& event, actor::Ship::InputStates& req)
{
    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_SPACE && !event.key.repeat)
        {
            req.is_firing = true;
        }
        if (event.key.keysym.sym == SDLK_x)
        {
            std::cout << "changed fire mode" << std::endl;
            req.changed_fire_mode = true;
        }
        if (!event.key.repeat && key_mapping.find(event.key.keysym.sym) != key_mapping.end())
        {
            req.motion_control_states[static_cast<int>(key_mapping.at(event.key.keysym.sym))] =
                true;
        }
    }
    else if (event.type == SDL_KEYUP)
    {
        if (event.key.keysym.sym == SDLK_SPACE)
        {
            req.is_firing = false;
        }
        else if (!event.key.repeat && key_mapping.find(event.key.keysym.sym) != key_mapping.end())
        {
            req.motion_control_states[static_cast<int>(key_mapping.at(event.key.keysym.sym))] =
                false;
        }
    }
}

int main(int argc, char* argv[])
{
    int screen_w = 1200;
    int screen_h = 900;

    convenience::init_sdl();
    rendering::ContextManager context_manager{ "Control Example", screen_w, screen_h };
    convenience::init_glew(screen_w, screen_h);

    glEnable(GL_BLEND);

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

    auto camera_controller =
        control::CameraController(awing.get_position(), awing.get_orientation());
    camera.set_tick_behavior([&awing, &camera_controller]() {
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
            awing.tick(t, dt);
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

        rendering::draw(shader_model,
                        rendering_manager.get_model(awing.get_visual_name()),
                        awing.get_goal_pose(),
                        { 1.0f, 0.0f, 0.0f },
                        rendering_manager.get_textures(),
                        GL_TRIANGLES);

        SDL_GL_SwapWindow(context_manager.window);

        // Process input
        actor::Ship::InputStates input_request;
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
            }
            update_input_request(event, input_request);
        }
        awing.update_input_states(input_request);
    }
}