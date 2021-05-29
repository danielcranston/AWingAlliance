#include <string>
#include <iostream>
#include <unordered_map>
#include <iomanip>

#include <SDL2/SDL.h>

#include "resources/locator.h"
#include "resources/load_descriptions.h"
#include "control/fire_control.h"
#include "audio/audio_manager.h"
#include "control/motion_control.h"

const std::unordered_map<int, control::MotionControl::States> key_mapping = {

    { SDLK_UP, control::MotionControl::States::TURN_UP },
    { SDLK_DOWN, control::MotionControl::States::TURN_DOWN },
    { SDLK_LEFT, control::MotionControl::States::TURN_LEFT },
    { SDLK_RIGHT, control::MotionControl::States::TURN_RIGHT },
    { SDLK_a, control::MotionControl::States::ROLL_LEFT },
    { SDLK_d, control::MotionControl::States::ROLL_RIGHT },
    { SDLK_w, control::MotionControl::States::ACC_INCREASE },
    { SDLK_s, control::MotionControl::States::ACC_DECREASE }
};

class FiringState
{
  public:
    FiringState(control::FireControl fire_control)
      : is_firing(false), audio_manager(resources::locator::SOUNDS_PATH), fire_control(fire_control)
    {
        audio_manager.loadAudio("XW_Laser_4A.wav", 0.2);
    };

    bool is_firing;
    AudioManager audio_manager;
    control::FireControl fire_control;

    void tick()
    {
        if (is_firing)
        {
            float now_s = SDL_GetTicks() / 1000.0f;
            auto dispatches = fire_control.fire(now_s);

            if (dispatches.has_value())
            {
                std::cout << "  Fired " << dispatches.value().size() << " shot(s) at time " << now_s
                          << std::endl;

                audio_manager.stop("XW_Laser_4A.wav");
                audio_manager.play("XW_Laser_4A.wav");
            }
        }
    }
};

int main(int argc, char* argv[])
{
    int screen_w = 1200;
    int screen_h = 900;

    if (SDL_Init(SDL_INIT_EVERYTHING | SDL_VIDEO_OPENGL) != 0)
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());

    auto window = SDL_CreateWindow("fire_control_example",
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   screen_w,
                                   screen_h,
                                   SDL_WINDOW_OPENGL);
    if (!window)
        throw std::runtime_error(std::string("Error creating SDL window: ") + SDL_GetError());
    auto context = SDL_GL_CreateContext(window);

    auto descriptions = resources::load_descriptions();
    auto f_control = descriptions.at("awing").primary_fire_control;
    auto fire_control = control::FireControl(f_control);
    FiringState firing_state(fire_control);

    auto motion_control = control::MotionControl();

    SDL_Event event;
    bool should_shutdown = false;
    while (!should_shutdown)
    {
        firing_state.tick();

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
                    std::cout << "Start firing" << std::endl;
                    firing_state.is_firing = true;
                }
                if (event.key.keysym.sym == SDLK_x && !event.key.repeat)
                {
                    std::cout << "Switched fire mode" << std::endl;
                    firing_state.fire_control.toggle_fire_mode();
                }
                if (!event.key.repeat &&
                    key_mapping.find(event.key.keysym.sym) != key_mapping.end())
                {
                    motion_control.turn_on(key_mapping.at(event.key.keysym.sym));
                    std::cout << motion_control.get_states().states << std::endl;
                }
            }
            if (event.type == SDL_KEYUP)
            {
                if (event.key.keysym.sym == SDLK_SPACE)
                {
                    std::cout << "Stop firing" << std::endl;
                    firing_state.is_firing = false;
                }
                if (!event.key.repeat &&
                    key_mapping.find(event.key.keysym.sym) != key_mapping.end())
                {
                    motion_control.turn_off(key_mapping.at(event.key.keysym.sym));
                    std::cout << motion_control.get_states().states << std::endl;
                }
            }
        }
    }
}