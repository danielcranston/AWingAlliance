#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include <GL/glew.h>

#include "data_handling.h"
#include "rendering/shader_program.h"

namespace rendering
{
namespace
{
void print_info_log(const unsigned int obj, const char* fn, const bool is_program = false)
{
    int infologLength = 0;
    is_program ? glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength) :
                 glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 2)
    {
        fprintf(stderr, "[From %s:]\n", fn);
        char* infoLog = (char*)malloc(infologLength);
        int charsWritten = 0;
        is_program ? glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog) :
                     glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        fprintf(stderr, "%s\n", infoLog);
        free(infoLog);
    }
    else
    {
        std::cout << "Tried to print shader info log, but no data could be extracted";
    }
}

/**
 * @brief RAII wrapper around an OpenGL shader object (vertex or fragment)
 */
struct LoadedShaderObject
{
    LoadedShaderObject(const std::string& filename, const unsigned int shader_object_type)
      : filename(filename)
    {
        std::cout << "Loading \"" << filename << "\" ..." << std::endl;

        const std::string shader_source =
            data_handling::load_textfile(filename, data_handling::SHADERS_PATH);

        shader = glCreateShader(shader_object_type);

        const char* shader_source_ptr = shader_source.c_str();
        glShaderSource(shader, 1, &shader_source_ptr, NULL);

        glCompileShader(shader);

        int is_compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
        if (is_compiled == GL_FALSE)
        {
            print_info_log(shader, filename.c_str());
            throw std::runtime_error("Unable to compile shader.");
        }
    }

    ~LoadedShaderObject()
    {
        glDeleteShader(shader);
    }

    std::string filename;
    unsigned int shader;
};

unsigned int init_shader_program(const LoadedShaderObject& vertex_shader,
                                 const LoadedShaderObject& fragment_shader)
{
    unsigned int program = glCreateProgram();

    glAttachShader(program, vertex_shader.shader);
    glAttachShader(program, fragment_shader.shader);

    glLinkProgram(program);

    int is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    if (is_linked == GL_FALSE)
    {
        print_info_log(
            program,
            ("ShaderProgram (" + vertex_shader.filename + ", " + fragment_shader.filename).c_str(),
            true);
        throw std::runtime_error("Unable to link shader program.");
    }

    glValidateProgram(program);
    glDetachShader(program, vertex_shader.shader);
    glDetachShader(program, fragment_shader.shader);

    return program;
}

}  // namespace

ShaderProgram::ShaderProgram(const std::string& uri,
                             const std::string& vertex_filename,
                             const std::string& fragment_filename)
  : uri(uri)
{
    const auto vertex_shader = LoadedShaderObject(vertex_filename, GL_VERTEX_SHADER);
    const auto fragment_shader = LoadedShaderObject(fragment_filename, GL_FRAGMENT_SHADER);

    program_id = init_shader_program(vertex_shader, fragment_shader);

    std::cout << "ShaderProgram(program_id=" << program_id << " " << vertex_shader.filename << " "
              << fragment_shader.filename << ") constructed" << std::endl;
}

ShaderProgram::~ShaderProgram()
{
    std::cout << "ShaderProgram(program_id=" << program_id << ") being cleaned up" << std::endl;
    glUseProgram(0);
    glDeleteProgram(program_id);
}

void ShaderProgram::use() const
{
    glUseProgram(program_id);
}

void ShaderProgram::setUniform1i(const std::string& name, const int value) const
{
    glUniform1i(glGetUniformLocation(program_id, name.c_str()), value);
}

void ShaderProgram::setUniform1f(const std::string& name, const float value) const
{
    glUniform1f(glGetUniformLocation(program_id, name.c_str()), value);
}

void ShaderProgram::setUniform2f(const std::string& name, const float f1, const float f2) const
{
    glUniform2f(glGetUniformLocation(program_id, name.c_str()), f1, f2);
}

void ShaderProgram::setUniform3fv(const std::string& name, const Eigen::Vector3f& vec) const
{
    glUniform3fv(glGetUniformLocation(program_id, name.c_str()), 1, vec.data());
}

void ShaderProgram::setUniformMatrix4fv(const std::string& name, const Eigen::Matrix4f& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, mat.data());
}

void ShaderProgram::setUniformMatrix3x4fv(const std::string& name,
                                          const Eigen::Matrix<float, 4, 3>& mat) const
{
    glUniformMatrix3x4fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, mat.data());
}

}  // namespace rendering
