#include <stdexcept>
#include <string>

#include "rendering/context_manager.h"
#include "rendering/convenience.h"

namespace rendering
{
ContextManager::ContextManager(const std::string& window_name,
                               const int screen_w,
                               const int screen_h)
  : screen_w(screen_w), screen_h(screen_h)
{
    convenience::init_sdl();
    window = SDL_CreateWindow(window_name.c_str(),
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              screen_w,
                              screen_h,
                              SDL_WINDOW_OPENGL);

    context = SDL_GL_CreateContext(window);
    if (!window)
        throw std::runtime_error(std::string("Error creating SDL window: ") + SDL_GetError());

    convenience::init_glew(screen_w, screen_h);
    convenience::init_imgui(window, context);
}
ContextManager::~ContextManager()
{
    ImPlot::DestroyContext();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ContextManager::imgui_new_frame() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}
void ContextManager::imgui_render() const
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void ContextManager::imgui_process_event(SDL_Event& event) const
{
    ImGui_ImplSDL2_ProcessEvent(&event);
}

}  // namespace rendering
