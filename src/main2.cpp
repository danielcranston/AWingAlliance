#include "rendering/context_manager.h"
#include "ecs/scene_factory.h"
#include "ecs/systems.h"
#include "input/key_event.h"

int main(int argc, char* argv[])
{
    auto context_manager = rendering::ContextManager::create("Main Window", 1200, 900);

    auto scene = ecs::SceneFactory::create_from_scenario("scenario");

    float current_time = SDL_GetTicks() / 1000.0f;
    const float dt = 1.0f / 60.f;
    float t = 0.0f;
    float accumulator = 0.0f;

    bool should_shutdown = false;
    while (!should_shutdown)
    {
        float new_time = SDL_GetTicks() / 1000.0f;
        float frameTime = new_time - current_time;
        current_time = new_time;

        accumulator += frameTime;

        while (accumulator >= dt)
        {
            ecs::systems::integrate(*scene, t, dt);
            accumulator -= dt;
            t += dt;
        }

        ecs::systems::render(*scene);

        SDL_GL_SwapWindow(context_manager.window);

        std::vector<KeyEvent> key_events;
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
            {
                should_shutdown = true;
            }
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP && !event.key.repeat)
            {
                key_events.emplace_back(static_cast<char>(event.key.keysym.sym),
                                        event.type == SDL_KEYDOWN ? KeyEvent::Status::PRESSED :
                                                                    KeyEvent::Status::RELEASED);
            }
        }

        ecs::systems::handle_key_events(*scene, key_events);
    }
}
