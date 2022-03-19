#pragma once

#include <string>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "implot/implot.h"
#include "implot/implot_internal.h"

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

    void imgui_new_frame() const;
    void imgui_render() const;
    void imgui_process_event(SDL_Event& event) const;

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
