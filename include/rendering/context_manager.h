#pragma once

#include <string>

#include <GL/glew.h>
#include <SDL2/SDL.h>

namespace rendering
{
class ContextManager
{
  public:
    ContextManager(const std::string& window_name, const int screen_w, const int screen_h);
    ~ContextManager();

    int screen_w;
    int screen_h;

    SDL_Window* window;
    SDL_GLContext context;

  private:
    ContextManager(ContextManager&) = delete;
    ContextManager(const ContextManager&) = delete;
    ContextManager(ContextManager&&) = delete;
    ContextManager(const ContextManager&&) = delete;
    ContextManager& operator=(ContextManager&) = delete;
    ContextManager& operator=(const ContextManager&) = delete;
    ContextManager& operator=(ContextManager&&) = delete;
    ContextManager& operator=(const ContextManager&&) = delete;
};
}  // namespace rendering
