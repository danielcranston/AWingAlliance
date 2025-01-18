#include "rendering/global.h"
#include "rendering/model.h"

#include <GL/glew.h>
#include <memory>

namespace rendering::global
{
struct UniformBufferObjectCameraMatrices;
struct UniformBufferObjectModelMatrices;

/**
 * @brief Hidden global state
 */

static std::unique_ptr<UniformBufferObjectCameraMatrices> UBO_CAMERAMATRICES = nullptr;
static std::unique_ptr<UniformBufferObjectModelMatrices> UBO_MODELMATRICES = nullptr;
std::unique_ptr<Mesh> QUAD_MESH = nullptr;
std::unique_ptr<Mesh> CUBE_MESH = nullptr;

static bool TEST_DEPTH_BUFFER = false;
static bool WRITE_DEPTH_BUFFER = false;
static bool CULL_BACK_FACES = false;

static unsigned int CURRENT_SHADER_PROGRAM = 9999;
static unsigned int CURRENT_UNIFORM_BUFFER = 9999;

/* End hidden global state */

namespace
{
// https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
unsigned int init_uniform_buffer_object(unsigned int binding_point, unsigned int buffer_size)
{
    unsigned int ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);

    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo);

    return ubo;
}
}  // namespace

struct UniformBufferObjectCameraMatrices
{
    UniformBufferObjectCameraMatrices()
    {
        ubo = init_uniform_buffer_object(0, 2 * 16 * sizeof(float));

        const Eigen::Matrix4f identity = Eigen::Matrix4f::Identity();

        glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float), identity.data());
        glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(float), 16 * sizeof(float), identity.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    unsigned int ubo;
};

struct UniformBufferObjectModelMatrices
{
    UniformBufferObjectModelMatrices()
    {
        ubo = init_uniform_buffer_object(1, 2 * 16 * sizeof(float));

        const Eigen::Matrix4f identity = Eigen::Matrix4f::Identity();

        glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float), identity.data());
        glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(float), 16 * sizeof(float), identity.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    unsigned int ubo;
};

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
    if (CURRENT_UNIFORM_BUFFER != UBO_CAMERAMATRICES->ubo)
    {
        CURRENT_UNIFORM_BUFFER = UBO_CAMERAMATRICES->ubo;
        glBindBuffer(GL_UNIFORM_BUFFER, UBO_CAMERAMATRICES->ubo);
    }

    glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float), mat.data());
}

void set_camera_pose(const Eigen::Matrix4f& mat)
{
    if (CURRENT_UNIFORM_BUFFER != UBO_CAMERAMATRICES->ubo)
    {
        CURRENT_UNIFORM_BUFFER = UBO_CAMERAMATRICES->ubo;
        glBindBuffer(GL_UNIFORM_BUFFER, UBO_CAMERAMATRICES->ubo);
    }

    glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(float), 16 * sizeof(float), mat.data());
}

void set_model_pose(const Eigen::Matrix4f& mat)
{
    if (CURRENT_UNIFORM_BUFFER != UBO_MODELMATRICES->ubo)
    {
        CURRENT_UNIFORM_BUFFER = UBO_MODELMATRICES->ubo;
        glBindBuffer(GL_UNIFORM_BUFFER, UBO_MODELMATRICES->ubo);
    }

    glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float), mat.data());
}

void set_model_scale(const Eigen::Vector3f& scale)
{
    if (CURRENT_UNIFORM_BUFFER != UBO_MODELMATRICES->ubo)
    {
        CURRENT_UNIFORM_BUFFER = UBO_MODELMATRICES->ubo;
        glBindBuffer(GL_UNIFORM_BUFFER, UBO_MODELMATRICES->ubo);
    }

    Eigen::Matrix4f mat = scale.homogeneous().asDiagonal().toDenseMatrix();
    glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(float), 16 * sizeof(float), mat.data());
}

}  // namespace rendering::global

namespace rendering::global::impl
{
void init()
{
    UBO_CAMERAMATRICES = std::make_unique<UniformBufferObjectCameraMatrices>();
    UBO_MODELMATRICES = std::make_unique<UniformBufferObjectModelMatrices>();

    QUAD_MESH = std::make_unique<Mesh>(std::move(Model("quad.obj").meshes[0]));
    CUBE_MESH = std::make_unique<Mesh>(std::move(Model("cube.obj").meshes[0]));

    write_depth_buffer(true);
    test_depth_buffer(true);
    cull_back_faces(true);
}
}  // namespace rendering::global::impl
