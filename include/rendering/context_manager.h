#pragma once

#include <memory>
#include <string>

class SDL_Window;
typedef void* SDL_GLContext;

namespace rendering
{
class ContextManager
{
  public:
    ContextManager(const std::string& window_name,
                   const int screen_w,
                   const int screen_h,
                   const bool headless = false);
    ~ContextManager();

    int screen_w;
    int screen_h;

    SDL_Window* window;
    std::unique_ptr<SDL_GLContext> context;

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
