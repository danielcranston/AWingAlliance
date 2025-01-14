#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

#include "rendering/context_manager.h"

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

namespace global
{
// https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
struct UniformBufferObject
{
    UniformBufferObject()
    {
        // Create buffer, bind it to Uniform Buffer 0
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, 2 * 16 * sizeof(float), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2 * 16 * sizeof(float));

        // Buffer some data into it
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);

        Eigen::Matrix4f persp = Eigen::Matrix4f::Identity();
        Eigen::Matrix4f camera = Eigen::Matrix4f::Identity();

        glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float), persp.data());
        glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(float), 16 * sizeof(float), camera.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    unsigned int ubo;
};

static std::unique_ptr<UniformBufferObject> UNIFORM_BUFFER_OBJECT = nullptr;
static bool TEST_DEPTH_BUFFER = true;
static bool WRITE_DEPTH_BUFFER = true;
static bool CULL_BACK_FACES = true;
static unsigned int CURENT_SHADER_PROGRAM = 9999;

void clear_frame(const bool color_buffer, const bool depth_buffer)
{
    unsigned int mask = color_buffer ? GL_COLOR_BUFFER_BIT : 0;
    mask |= depth_buffer ? GL_DEPTH_BUFFER_BIT : 0;
    glClear(mask);
}

void write_depth_buffer(const bool enable)
{
    if (WRITE_DEPTH_BUFFER != enable)
    {
        WRITE_DEPTH_BUFFER = enable;
        glDepthMask(enable);
    }
}

void test_depth_buffer(const bool enable)
{
    if (TEST_DEPTH_BUFFER != enable)
    {
        TEST_DEPTH_BUFFER = enable;
        enable ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    }
}

void cull_back_faces(const bool enable)
{
    if (CULL_BACK_FACES != enable)
    {
        CULL_BACK_FACES = enable;
        enable ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    }
}

void use_program(const ShaderProgram& shader_program)
{
    if (shader_program.program_id != CURENT_SHADER_PROGRAM)
    {
        glUseProgram(shader_program.program_id);
        CURENT_SHADER_PROGRAM = shader_program.program_id;
    }
}

void set_camera_perspective(const Eigen::Matrix4f& mat)
{
    glBindBuffer(GL_UNIFORM_BUFFER, UNIFORM_BUFFER_OBJECT->ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float), mat.data());
}

void set_camera_pose(const Eigen::Matrix4f& mat)
{
    glBindBuffer(GL_UNIFORM_BUFFER, UNIFORM_BUFFER_OBJECT->ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(float), 16 * sizeof(float), mat.data());
}

}  // namespace global

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

    global::UNIFORM_BUFFER_OBJECT = std::make_unique<global::UniformBufferObject>();

    glDepthMask(true);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

ContextManager::~ContextManager()
{
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

}  // namespace rendering
