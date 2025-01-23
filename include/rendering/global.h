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
void use_alpha(const bool enable);
void use_program(const ShaderProgram& shader_program);

void set_camera_perspective(const Eigen::Matrix4f& mat);
void set_camera_pose(const Eigen::Matrix4f& mat);
void set_model_pose(const Eigen::Matrix4f& mat);
void set_model_scale(const Eigen::Vector3f& scale);
void set_model_color(const Eigen::Vector3f& color);
void set_model_alpha(const float alpha);
void set_model_use_texture(const bool data);
void set_effect_current_time(const float time);
void set_effect_start_time(const float time);
void set_effect_resolution(const int weight, const int height);
void set_effect_num_layers(const int num_layers);

void register_custom_shader(const std::string& uri,
                            const std::string& vertex_filename,
                            const std::string& fragment_filename);

}  // namespace rendering::global

namespace rendering::global::impl
{
void init(const int screen_w, const int screen_h);  // Not intended to be called by user code
}
