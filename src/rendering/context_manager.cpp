#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

#include "rendering/context_manager.h"
#include "rendering/global.h"

namespace rendering
{
namespace
{
void init_sdl()
{
    if (SDL_Init(SDL_INIT_EVERYTHING | SDL_VIDEO_OPENGL) != 0)
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    const auto context_flags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | SDL_GL_CONTEXT_DEBUG_FLAG;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, context_flags);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    SDL_ShowCursor(true);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void init_glew(const int screen_w, const int screen_h)
{
    unsigned int glew_status = glewInit();
    if (glew_status != GLEW_OK)
    {
        std::stringstream ss;
        ss << "Error initializing GLEW: " << glewGetErrorString(glew_status);
        throw std::runtime_error(ss.str());
    }

    std::cout << "GLEW Initialized: " << glewGetErrorString(glew_status) << std::endl;

    glViewport(0, 0, screen_w, screen_h);
    glClearColor(0.1, 0.1, 0.1, 1);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
}  // namespace

ContextManager::ContextManager(const std::string& window_name,
                               const int screen_w,
                               const int screen_h)
  : screen_w(screen_w), screen_h(screen_h)
{
    init_sdl();
    window = SDL_CreateWindow(window_name.c_str(),
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              screen_w,
                              screen_h,
                              SDL_WINDOW_OPENGL);

    context = SDL_GL_CreateContext(window);
    if (!window)
    {
        throw std::runtime_error(std::string("Error creating SDL window: ") + SDL_GetError());
    }

    init_glew(screen_w, screen_h);

    global::impl::init(screen_w, screen_h);
}

ContextManager::~ContextManager()
{
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

}  // namespace rendering
