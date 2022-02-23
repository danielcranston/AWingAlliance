#include <exception>
#include <string>
#include <iostream>
#include <sstream>

#include "rendering/context_manager.h"
#include "rendering/convenience.h"

namespace rendering
{
ContextManager ContextManager::create(const std::string& window_name,
                                      const int screen_w,
                                      const int screen_h)
{
    convenience::init_sdl();
    return ContextManager(window_name, screen_w, screen_h);
}

ContextManager::ContextManager(const std::string& window_name,
                               const int screen_w,
                               const int screen_h)
  : screen_w(screen_w),
    screen_h(screen_h),
    window(SDL_CreateWindow(window_name.c_str(),
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            screen_w,
                            screen_h,
                            SDL_WINDOW_OPENGL)),  //| SDL_WINDOW_RESIZABLE);
    context(SDL_GL_CreateContext(window))
{
    if (!window)
        throw std::runtime_error(std::string("Error creating SDL window: ") + SDL_GetError());

    convenience::init_glew(screen_w, screen_h);
}
ContextManager::~ContextManager()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

}  // namespace rendering
