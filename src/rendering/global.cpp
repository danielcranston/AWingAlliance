#include "rendering/global.h"

#include <GL/glew.h>
#include <memory>

namespace rendering::global
{
namespace
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

/**
 * @brief Hidden global state
 */
static std::unique_ptr<UniformBufferObject> UNIFORM_BUFFER_OBJECT = nullptr;

static bool TEST_DEPTH_BUFFER = false;
static bool WRITE_DEPTH_BUFFER = false;
static bool CULL_BACK_FACES = false;

static unsigned int CURRENT_SHADER_PROGRAM = 9999;

}  // namespace

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
    if (shader_program.program_id != CURRENT_SHADER_PROGRAM)
    {
        glUseProgram(shader_program.program_id);
        CURRENT_SHADER_PROGRAM = shader_program.program_id;
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

namespace impl
{
void init()
{
    UNIFORM_BUFFER_OBJECT = std::make_unique<UniformBufferObject>();

    write_depth_buffer(true);
    test_depth_buffer(true);
    cull_back_faces(true);
}
}  // namespace impl

}  // namespace rendering::global