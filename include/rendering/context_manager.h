#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>

namespace rendering
{
struct ContextManager
{
    ContextManager(const std::string& window_name, const int screen_w, const int screen_h);
    ~ContextManager();

    ContextManager(ContextManager&) = delete;
    ContextManager(const ContextManager&) = delete;
    ContextManager(ContextManager&&) = delete;
    ContextManager(const ContextManager&&) = delete;
    ContextManager& operator=(ContextManager&) = delete;
    ContextManager& operator=(const ContextManager&) = delete;
    ContextManager& operator=(ContextManager&&) = delete;
    ContextManager& operator=(const ContextManager&&) = delete;

    SDL_Window* window;
    SDL_GLContext context;

    int screen_w;
    int screen_h;
};
}  // namespace rendering
