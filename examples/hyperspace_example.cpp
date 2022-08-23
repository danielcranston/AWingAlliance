#include <iostream>
#include <string>
#include <map>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "audio/audio.h"
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
      : transition_shader_ptr(transition_shader_ptr),
        during_shader_ptr(during_shader_ptr),
        current_state(State::IDLE),
        epoch(0)
    {
        loadAudio("Computer_Drone_10.wav", 0.1);
        loadAudio("Fighter_Startup_2.wav", 1.0);
        loadAudio("Hyperspace_Start.wav", 1.0);
        loadAudio("Deflector_Hit_1B.wav", 1.0);
        loadAudio("XW_ENG_11.wav", 0.25);
        loadAudio("Laser_Whoosh_4A.wav", 0.25);

        SetState(State::IDLE);
    };

    void Tick(float ms) const
    {
        now = (ms - epoch) / 1000.0f;
        switch (current_state)
        {
            case State::IDLE:
                playAudio("Computer_Drone_10.wav");
                break;
            case State::ENTERING:
                if (now < 4.5f)
                {
                    transition_shader_ptr->setUniform1f("time", now);

                    if (now < 0.5)
                    {
                        playAudio("Fighter_Startup_2.wav");
                    }
                    if (now > 1.75f)
                    {
                        playAudio("Hyperspace_Start.wav");
                    }
                    if (now > 4.2f)
                    {
                        playAudio("Deflector_Hit_1B.wav");
                    }
                }
                else
                {
                    stopAudio("Computer_Drone_10.wav");
                    stopAudio("Fighter_Startup_2.wav");
                    SetState(State::DURING);
                }
                break;
            case State::DURING:
                during_shader_ptr->setUniform1f("time", now);

                playAudio("XW_ENG_11.wav");
                break;
            case State::EXITING:
                stopAudio("XW_ENG_11.wav");
                if (now < 3.5f)
                {
                    transition_shader_ptr->setUniform1f("time", now + 4.5f);
                    if (now < 1.0f)
                    {
                        playAudio("Laser_Whoosh_4A.wav");
                    }
                    if (now > 1.0f)
                    {
                        playAudio("Computer_Drone_10.wav");
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

    using SourceBufferPair =
        std::pair<std::unique_ptr<audio::AudioBuffer>, std::unique_ptr<audio::AudioSource>>;
    std::map<std::string, SourceBufferPair> sounds;

    void loadAudio(const std::string& name, const float& gain)
    {
        sounds.insert(
            std::make_pair(name,
                           std::make_pair(std::make_unique<audio::AudioBuffer>(
                                              resources::locator::SOUNDS_PATH + name),
                                          std::make_unique<audio::AudioSource>(gain, false))));
    }

    void playAudio(const std::string& name) const
    {
        auto& pair = sounds.at(name);
        if (!pair.second->is_playing())
        {
            pair.second->play(*pair.first);
        }
    }

    void stopAudio(const std::string& name) const
    {
        auto& pair = sounds.at(name);
        pair.second->stop();
    }

  private:
    const std::string sounds_path;
    const rendering::ShaderProgram* transition_shader_ptr;
    const rendering::ShaderProgram* during_shader_ptr;
};

int main(int argc, char* argv[])
{
    int screen_w = 1200;
    int screen_h = 900;

    rendering::ContextManager context_manager{ "Hyperspace Example", screen_w, screen_h };

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
