#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <Eigen/Geometry>

#include "rendering/shader_program.h"

/**
 * @brief This namespace holds the global OpenGL state. How this state is maintained is
 * implementation details. Users may change the global state only via the functions exposed below.
 */
namespace rendering::global
{

void clear_frame(const bool color_buffer, const bool depth_buffer);
void write_depth_buffer(const bool enable);
void test_depth_buffer(const bool enable);
void cull_back_faces(const bool enable);
void use_program(const ShaderProgram& shader_program);

void set_camera_perspective(const Eigen::Matrix4f& mat);
void set_camera_pose(const Eigen::Matrix4f& mat);

}  // namespace rendering::global

namespace rendering::global::impl
{
void init();  // Not intended to be called by user code
}
