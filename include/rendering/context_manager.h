#pragma once

#include <string>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <Eigen/Dense>

#include "rendering/shader_program.h"

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

/**
 * @brief This namespace holds the global OpenGL state. Its exact contents are implementation
 * details. Users may change the global state only via the functions exposed below.
 */
namespace global
{

void clear_frame(const bool color_buffer, const bool depth_buffer);
void write_depth_buffer(const bool enable);
void test_depth_buffer(const bool enable);
void cull_back_faces(const bool enable);
void use_program(const ShaderProgram& shader_program);

void set_camera_perspective(const Eigen::Matrix4f& mat);
void set_camera_pose(const Eigen::Matrix4f& mat);

}  // namespace global
}  // namespace rendering
