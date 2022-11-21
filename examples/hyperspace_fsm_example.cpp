#include <iostream>
#include <string>
#include <map>

#include "audio/audio.h"
#include "resources/locator.h"
#include "rendering/primitives.h"
#include "rendering/compile_shader_program.h"
#include "rendering/context_manager.h"
#include "ecs/resource_manager.h"
#define HFSM2_ENABLE_PLAN
#include <hfsm2/machine.hpp>

inline float current_time()
{
    return SDL_GetTicks() / 1000.0f;
}

struct Context
{
    ecs::ResourceManager* resource_manager;
    entt::resource_handle<const rendering::ShaderProgram> tunnel_shader;
    entt::resource_handle<const rendering::ShaderProgram> transition_shader;

    float last_transition_time = 0.0f;

    std::shared_ptr<audio::AudioSource> sound_source_idle;
    std::shared_ptr<audio::AudioSource> sound_source_effect1;
    std::shared_ptr<audio::AudioSource> sound_source_effect2;

    float time_since_last_transition() const
    {
        return current_time() - last_transition_time;
    }

    void use_tunnel_shader()
    {
    }
};

using M = hfsm2::MachineT<hfsm2::Config::ContextT<Context>>;

struct IdleState;
struct EnteringStateStartup;
struct EnteringStateHyperspaceStart;
struct EnteringStateHyperspaceBreakthrough;
struct DuringState;
struct ExitingState;

using FSM = M::PeerRoot<IdleState,
                        EnteringStateStartup,
                        EnteringStateHyperspaceStart,
                        EnteringStateHyperspaceBreakthrough,
                        DuringState,
                        ExitingState>;

struct Event
{
};

struct IdleState : FSM::State
{
    void enter(Control& control) noexcept
    {
        auto& context = control.context();
        context.last_transition_time = current_time();
        context.transition_shader->use();

        context.sound_source_idle->play(
            context.resource_manager->get_sound("Computer_Drone_10.wav").get(), 0.1);

        std::cout << "  IdleState" << std::endl;
    }

    void react(const Event&, FullControl& control)
    {
        control.changeTo<EnteringStateStartup>();
    }
};

struct EnteringStateStartup : FSM::State
{
    void enter(Control& control) noexcept
    {
        auto& context = control.context();
        context.last_transition_time = current_time();

        context.sound_source_effect1->play(
            context.resource_manager->get_sound("Fighter_Startup_2.wav").get());

        std::cout << "  EnteringStateStartup" << std::endl;
    }

    void update(FullControl& control)
    {
        const auto& context = control.context();

        const float time_spent_in_state = context.time_since_last_transition();
        context.transition_shader->setUniform1f("time", time_spent_in_state);

        if (time_spent_in_state > 1.75f)
        {
            control.changeTo<EnteringStateHyperspaceStart>();
        }
    }
};

struct EnteringStateHyperspaceStart : FSM::State
{
    void enter(Control& control) noexcept
    {
        auto& context = control.context();

        context.sound_source_effect2->play(
            context.resource_manager->get_sound("Hyperspace_Start.wav").get());

        std::cout << "  EnteringStateHyperspaceStart" << std::endl;
    }

    void update(FullControl& control)
    {
        const auto& context = control.context();

        const float time_spent_in_state = context.time_since_last_transition();
        context.transition_shader->setUniform1f("time", time_spent_in_state);

        if (time_spent_in_state > 4.2f)
        {
            control.changeTo<EnteringStateHyperspaceBreakthrough>();
        }
    }
};

struct EnteringStateHyperspaceBreakthrough : FSM::State
{
    void enter(Control& control) noexcept
    {
        auto& context = control.context();

        context.sound_source_idle->play(context.resource_manager->get_sound("XW_ENG_11.wav").get(),
                                        0.25);
        context.sound_source_effect1->play(
            context.resource_manager->get_sound("Deflector_Hit_1B.wav").get());

        std::cout << "  EnteringStateHyperspaceBreakthrough" << std::endl;
    }

    void update(FullControl& control)
    {
        const auto& context = control.context();

        const float time_spent_in_state = context.time_since_last_transition();
        context.transition_shader->setUniform1f("time", time_spent_in_state);

        if (time_spent_in_state > 4.5)
        {
            control.changeTo<DuringState>();
        }
    }
};

struct DuringState : FSM::State
{
    void enter(Control& control) noexcept
    {
        auto& context = control.context();
        context.last_transition_time = current_time();

        context.tunnel_shader->use();

        std::cout << "  DuringState" << std::endl;
    }

    void update(FullControl& control)
    {
        auto& context = control.context();
        context.tunnel_shader->setUniform1f("time", context.time_since_last_transition());
    }

    void react(const Event&, FullControl& control)
    {
        if (control.context().time_since_last_transition() > 1.0f)
        {
            control.changeTo<ExitingState>();
        }
    }
};

struct ExitingState : FSM::State
{
    void enter(Control& control) noexcept
    {
        auto& context = control.context();
        context.last_transition_time = current_time();

        context.transition_shader->use();

        context.sound_source_idle->play(
            context.resource_manager->get_sound("Computer_Drone_10.wav").get(), 0.1);
        context.sound_source_effect1->play(
            context.resource_manager->get_sound("Laser_Whoosh_4A.wav").get(), 0.25);

        std::cout << "  ExitingState" << std::endl;
    }

    void update(FullControl& control)
    {
        const auto& context = control.context();
        const float time_spent_in_state = context.time_since_last_transition();

        context.transition_shader->setUniform1f("time", time_spent_in_state + 4.5f);
        if (time_spent_in_state > 3.5f)
        {
            control.changeTo<IdleState>();
        }
    }
};

int main(int argc, char* argv[])
{
    int screen_w = 1200;
    int screen_h = 900;

    rendering::ContextManager context_manager{ "Hyperspace Example", screen_w, screen_h };

    ecs::ResourceManager resource_manager;

    resource_manager.load_sound("Computer_Drone_10.wav");
    resource_manager.load_sound("Fighter_Startup_2.wav");
    resource_manager.load_sound("Hyperspace_Start.wav");
    resource_manager.load_sound("Deflector_Hit_1B.wav");
    resource_manager.load_sound("XW_ENG_11.wav");
    resource_manager.load_sound("Laser_Whoosh_4A.wav");

    resource_manager.load_shader("hyperspace_tunnel", "hyperspace.vert", "hyperspace_tunnel.frag");
    resource_manager.load_shader("hyperspace_jump", "hyperspace.vert", "hyperspace_jump.frag");

    auto tunnel_shader = resource_manager.get_shader("hyperspace_tunnel");
    tunnel_shader->use();
    tunnel_shader->setUniformMatrix4fv("matrix", Eigen::Matrix4f::Identity());
    tunnel_shader->setUniform2f(
        "resolution", static_cast<float>(screen_w), static_cast<float>(screen_h));
    float hyperspace_scale = 40.0f;
    tunnel_shader->setUniform1f("hyperspace_scale", 40.0f);

    auto transition_shader = resource_manager.get_shader("hyperspace_jump");
    transition_shader->use();
    transition_shader->setUniform2f(
        "resolution", static_cast<float>(screen_w), static_cast<float>(screen_h));

    Context context;
    context.resource_manager = &resource_manager;
    context.tunnel_shader = tunnel_shader;
    context.transition_shader = transition_shader;

    context.sound_source_idle = std::make_shared<audio::AudioSource>(1.0f, true);
    context.sound_source_effect1 = std::make_shared<audio::AudioSource>(1.0f);
    context.sound_source_effect2 = std::make_shared<audio::AudioSource>(1.0f);

    const auto quad = rendering::primitives::quad();
    glBindVertexArray(quad.get_meshes()[0].get_vao());

    std::cout << "\nPress Space to enter/exit hyperspace" << std::endl;

    FSM::Instance fsm{ context };

    SDL_Event event;
    bool should_shutdown = false;

    while (!should_shutdown)
    {
        fsm.update();

        glDrawElements(
            GL_TRIANGLES, quad.get_meshes()[0].get_num_indices(), GL_UNSIGNED_INT, (const void*)0);

        SDL_GL_SwapWindow(context_manager.window);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                should_shutdown = true;
            }
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    should_shutdown = true;
                }
                if (event.key.keysym.sym == SDLK_SPACE && !event.key.repeat)
                {
                    fsm.react(Event());
                }
            }
        }
    }
}
