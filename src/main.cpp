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
#include "geometry/geometry.h"
#include "actor/actor.h"
#include "actor/ship.h"
#include "actor/camera.h"
#include "actor/laser.h"
#include "control/camera_controller.h"
#include "environment/environment.h"
#include "convenience.h"

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

class StatfulKalmanPositioner
{
  public:
    StatfulKalmanPositioner() = default;

    Eigen::Isometry3f update(const Eigen::Vector3f& pos, const Eigen::Quaternionf quat, float dt)
    {
        float angle = _o.angularDistance(quat);

        auto d_pos = pos - _p;

        auto a = d_pos;
        auto b = d_pos.dot(_vel) * d_pos.normalized();

        _vel = _vel + (a + b) / 2.0f * 0.05f * dt;

        _vel *= 0.5;

        _p = _p + _vel;

        _o = _o.slerp(0.1f, quat).normalized();

        auto ret = Eigen::Isometry3f(_o);
        ret.translation() = _p;
        return ret;
    }

    Eigen::Vector3f _p = Eigen::Vector3f::Zero();
    Eigen::Vector3f _vel = Eigen::Vector3f::Zero();
    Eigen::Quaternionf _o = Eigen::Quaternionf::Identity();
};

int main(int argc, char* argv[])
{
    int screen_w = 1200;
    int screen_h = 900;

    convenience::init_sdl();
    rendering::ContextManager context_manager{ "Main Window", screen_w, screen_h };
    convenience::init_glew(screen_w, screen_h);

    float FOV_Y = 60.0f * M_PI / 180.0f;
    float aspect = 1.0f * screen_w / screen_h;
    const auto perspective = geometry::perspective(FOV_Y, aspect, 5.0f, 8192.0f);
    const std::string skybox_tex = "skybox/new";

    auto environment = environment::Environment();

    actor::Ship::set_on_fire_cb(
        [&environment](const actor::Ship& ship, const Eigen::Isometry3f relative_pose) {
            const auto pose = ship.get_pose() * relative_pose;
            environment.register_actor(actor::Laser("laser",
                                                    pose.translation(),
                                                    Eigen::Quaternionf(pose.linear()),
                                                    Eigen::Vector3f(0.15f, 0.15f, 4.0f),
                                                    Eigen::Vector3f(1.0f, 0.0f, 0.0f),
                                                    1000.0f));
        });

    auto desc = resources::load_descriptions();

    environment.register_actor(
        actor::Ship("ship", desc.at("awing"), { 0.0f, -5.0f, -30.0f }, { 1.0f, 0.0f, 0.0f, 0.0f }));
    environment.register_actor(
        actor::Actor("sd", { 0.0f, -0.0f, -100.0f }, { 0.0f, 0.0f, 1.0f, 0.0f }, "sd.obj"));
    environment.register_actor(actor::Actor(
        "medfrigate", { 256.0f, 100.0f, -50.0f }, { 0.0f, 0.0f, 1.0f, 0.0f }, "medfrigate.obj"));
    environment.register_actor(
        actor::Camera("camera", { 0.0f, -0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 0.0f }, perspective));

    actor::Ship& ship = environment.get_actor<actor::Ship>("ship");
    actor::Actor& sd = environment.get_actor<actor::Actor>("sd");
    actor::Camera& camera = environment.get_actor<actor::Camera>("camera");

    std::reference_wrapper<actor::Actor> controlled_actor = ship;

    auto camera_controller = control::CameraController(controlled_actor.get().get_position(),
                                                       controlled_actor.get().get_orientation());
    camera.set_tick_behavior([&controlled_actor, &camera_controller]() {
        return controlled_actor.get().get_pose() * geometry::make_pose({ 0.0f, 5.0f, 15.0f });
    });
    auto rendering_manager = rendering::RenderingManager();

    auto shader_setup_fn = [&perspective](rendering::ShaderProgram* program) {
        program->use();
        program->setUniform1i("tex", 0);
        program->setUniformMatrix4fv("perspective", perspective);
        program->setUniformMatrix4fv("model_scale", Eigen::Matrix4f::Identity());
    };

    rendering_manager.register_shader_program("model", "model.vert", "model.frag", shader_setup_fn);
    auto& shader_model = rendering_manager.get_shader_program("model");

    rendering_manager.register_shader_program("skybox", "sky.vert", "sky.frag", shader_setup_fn);
    auto& shader_skybox = rendering_manager.get_shader_program("skybox");

    rendering_manager.register_model(rendering::primitives::bounding_box());
    rendering_manager.register_model(rendering::primitives::box());

    rendering_manager.register_models(environment.get_visuals(), [](const std::string& filename) {
        return resources::load_model(filename);
    });

    rendering_manager.register_skybox(skybox_tex, resources::load_cubemap_texture(skybox_tex));

    rendering_manager.register_unloaded_textures(
        [](const std::string& filename) { return resources::load_texture(filename); });

    bool should_shutdown = false;

    float current_time = SDL_GetTicks() / 1000.0f;
    const float dt = 1.0f / 60.f;
    float t = 0.0f;
    float accumulator = 0.0f;

    while (!should_shutdown)
    {
        float new_time = SDL_GetTicks() / 1000.0f;
        float frameTime = new_time - current_time;
        current_time = new_time;

        accumulator += frameTime;

        while (accumulator >= dt)
        {
            environment.integrate(t, dt);
            accumulator -= dt;
            t += dt;
        }

        // Draw Skybox
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

        // Draw Actors and derived classes
        shader_model.use();
        shader_model.setUniformMatrix4fv("camera", camera.get_pose().matrix().inverse());

        for (auto& [name, actor] : environment.get_actors())
        {
            rendering::draw(shader_model,
                            rendering_manager.get_model(actor.get_visual_name()),
                            actor.get_pose(),
                            { 1.0f, 0.0f, 0.0f },
                            rendering_manager.get_textures(),
                            GL_TRIANGLES);
        }

        for (auto& [name, ship] : environment.get_ships())
        {
            rendering::draw(shader_model,
                            rendering_manager.get_model(ship.get_visual_name()),
                            ship.get_pose(),
                            { 1.0f, 0.0f, 0.0f },
                            rendering_manager.get_textures(),
                            GL_TRIANGLES);
        }

        auto& lasers = environment.get_lasers();
        if (lasers.size() > 0)
        {
            shader_model.setUniformMatrix4fv("model_scale", lasers.front().get_scale());
            for (auto& laser : environment.get_lasers())
            {
                rendering::draw(shader_model,
                                rendering_manager.get_model("box"),
                                laser.get_pose(),
                                { 1.0f, 0.0f, 0.0f },
                                rendering_manager.get_textures(),
                                GL_TRIANGLES);
            }
        }

        // Draw bounding box, just because
        auto bb_scale = geometry::to_scale_matrix(
            rendering_manager.get_model("sd.obj").get_bounding_box().sizes());
        shader_model.setUniformMatrix4fv("model_scale", bb_scale);
        rendering::draw(shader_model,
                        rendering_manager.get_model("bounding_box"),
                        sd.get_pose(),
                        { 1.0f, 0.0f, 0.0f },
                        rendering_manager.get_textures(),
                        GL_LINES);

        shader_model.setUniformMatrix4fv("model_scale", Eigen::Matrix4f::Identity());

        SDL_GL_SwapWindow(context_manager.window);

        actor::Ship::InputStates input_request;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
            {
                should_shutdown = true;
            }

            update_input_request(event, input_request);

            /*
            else if (event.type == SDL_MOUSEMOTION)
            {
                int x, y;
                SDL_GetRelativeMouseState(&x, &y);

                controlled_actor.get().set_orientation(
                    Eigen::AngleAxis<float>(-M_PI * dt * 0.25f * x, Eigen::Vector3f::UnitY()) *
                    controlled_actor.get().get_orientation());

                auto a = Eigen::Quaternionf(Eigen::AngleAxis<float>(
                    -M_PI * dt * 0.25f * y, controlled_actor.get().get_right_dir()));
                auto b = controlled_actor.get().get_orientation();

                controlled_actor.get().set_orientation((a * b).normalized());

                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    std::cout << "  yep" << std::endl;
                }
            } */
        }

        ship.update_input_states(input_request);
    }

    return 0;
}
