#include <iostream>
#include <string>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "SDL2/SDL_mixer.h"

#include "audio/audio_manager.h"
#include "resources/locator.h"
#include "rendering/primitives.h"
#include "rendering/compile_shader_program.h"
#include "rendering/context_manager.h"

class HyperSpaceRendering
{
  public:
    enum class State
    {
        IDLE,
        ENTERING,
        DURING,
        EXITING
    };
    HyperSpaceRendering(const rendering::ShaderProgram* transition_shader_ptr,
                        const rendering::ShaderProgram* during_shader_ptr)
      : audio_manager(resources::locator::SOUNDS_PATH),
        transition_shader_ptr(transition_shader_ptr),
        during_shader_ptr(during_shader_ptr),
        current_state(State::IDLE),
        epoch(0)
    {
        audio_manager.loadAudio("Computer_Drone_10.wav", 0.1);  //, 16
        audio_manager.loadAudio("Fighter_Startup_2.wav");
        audio_manager.loadAudio("Hyperspace_Start.wav");
        audio_manager.loadAudio("Deflector_Hit_1B.wav");
        audio_manager.loadAudio("XW_ENG_11.wav", 0.25);        //, 64
        audio_manager.loadAudio("Laser_Whoosh_4A.wav", 0.25);  //, 64

        SetState(State::IDLE);
    };

    void Tick(float ms) const
    {
        now = (ms - epoch) / 1000.0f;
        switch (current_state)
        {
            case State::IDLE:
                audio_manager.play("Computer_Drone_10.wav");
                break;
            case State::ENTERING:
                if (now < 4.5f)
                {
                    transition_shader_ptr->setUniform1f("time", now);

                    if (now < 0.5)
                    {
                        audio_manager.play("Fighter_Startup_2.wav");
                    }
                    if (now > 1.75f)
                    {
                        audio_manager.play("Hyperspace_Start.wav");
                    }
                    if (now > 4.2f)
                    {
                        audio_manager.play("Deflector_Hit_1B.wav");
                    }
                }
                else
                {
                    audio_manager.stop("Computer_Drone_10.wav");
                    audio_manager.stop("Fighter_Startup_2.wav");
                    SetState(State::DURING);
                }
                break;
            case State::DURING:
                during_shader_ptr->setUniform1f("time", now);

                audio_manager.play("XW_ENG_11.wav");
                break;
            case State::EXITING:
                audio_manager.stop("XW_ENG_11.wav");
                if (now < 3.5f)
                {
                    transition_shader_ptr->setUniform1f("time", now + 4.5f);
                    if (now < 1.0f)
                    {
                        audio_manager.play("Laser_Whoosh_4A.wav");
                    }
                    if (now > 1.0f)
                    {
                        audio_manager.play("Computer_Drone_10.wav");
                    }
                }
                else
                {
                    SetState(State::IDLE);
                }
                break;
        }
    }

    void SetState(State state) const
    {
        current_state = state;
        epoch = SDL_GetTicks();

        switch (current_state)
        {
            case State::IDLE:
                transition_shader_ptr->use();
                break;
            case State::ENTERING:
                transition_shader_ptr->use();
                break;
            case State::DURING:
                during_shader_ptr->use();
                break;
            case State::EXITING:
                transition_shader_ptr->use();
                break;
        }
    }
    State GetCurrentState() const
    {
        return current_state;
    }

    mutable State current_state;
    mutable std::size_t epoch;
    mutable float now;

    mutable AudioManager audio_manager;

  private:
    const std::string sounds_path;
    const rendering::ShaderProgram* transition_shader_ptr;
    const rendering::ShaderProgram* during_shader_ptr;
};

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

    rendering::ContextManager context_manager{ "Hyperspace Example", screen_w, screen_h };

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
    glClearColor(0.5, 0, 0, 1);
    auto tunnel_shader =
        rendering::compileShaders("hyperspace_tunnel", "hyperspace.vert", "hyperspace_tunnel.frag");
    tunnel_shader->use();
    tunnel_shader->setUniformMatrix4fv("matrix", Eigen::Matrix4f::Identity());
    tunnel_shader->setUniform2f(
        "resolution", static_cast<float>(screen_w), static_cast<float>(screen_h));
    float hyperspace_scale = 40.0f;
    tunnel_shader->setUniform1f("hyperspace_scale", 40.0f);

    auto transition_shader =
        rendering::compileShaders("hyperspace_jump", "hyperspace.vert", "hyperspace_jump.frag");
    transition_shader->use();
    transition_shader->setUniform2f(
        "resolution", static_cast<float>(screen_w), static_cast<float>(screen_h));

    const auto hyperspace = HyperSpaceRendering(transition_shader.get(), tunnel_shader.get());

    const auto quad = rendering::primitives::quad();

    SDL_Event event;
    bool should_shutdown = false;

    std::cout << "\nPress Space to enter/exit hyperspace" << std::endl;

    while (!should_shutdown)
    {
        hyperspace.Tick(static_cast<float>(SDL_GetTicks()));

        glBindVertexArray(quad.get_meshes()[0].get_vao());
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
                    if (hyperspace.GetCurrentState() == HyperSpaceRendering::State::IDLE)
                    {
                        hyperspace.SetState(HyperSpaceRendering::State::ENTERING);
                    }
                    else if (hyperspace.GetCurrentState() == HyperSpaceRendering::State::DURING)
                    {
                        hyperspace.SetState(HyperSpaceRendering::State::EXITING);
                    }
                }
            }
        }
    }
}
