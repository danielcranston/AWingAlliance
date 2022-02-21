#include <iostream>
#include <vector>
#include <list>
#include <map>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <GL/glew.h>

#include <SDL2/SDL.h>

#include <entt/entt.hpp>

#include "resources/load_model.h"
#include "resources/load_texture.h"
#include "resources/load_geometry.h"
#include "rendering/model.h"
#include "rendering/mesh.h"
#include "rendering/compile_shader_program.h"
#include "rendering/primitives.h"
#include "rendering/context_manager.h"
#include "rendering/draw.h"
#include "geometry/geometry.h"
#include "geometry/collision.h"
#include "control/camera_controller.h"
#include "convenience.h"
#include "scene.h"
#include "ecs/components.h"
#include "ecs/systems.h"

int main(int argc, char* argv[])
{
    int screen_w = 1200;
    int screen_h = 900;

    convenience::init_sdl();
    rendering::ContextManager context_manager{ "Main Window", screen_w, screen_h };
    convenience::init_glew(screen_w, screen_h);

    auto scene = Scene::load_from_scenario("scenario");

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
            ecs::systems::integrate(*scene, dt);
            accumulator -= dt;
            t += dt;
        }

        ecs::systems::render(*scene);

        SDL_GL_SwapWindow(context_manager.window);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
            {
                should_shutdown = true;
            }
        }
    }
}
