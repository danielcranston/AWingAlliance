#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include <GL/glew.h>

#include "resources/locator.h"
#include "resources/load_textfile.h"
#include "rendering/shader_program.h"

namespace rendering
{
ShaderProgram::ShaderProgram(const std::string& n, const ShaderType t, const uint program)
  : name(n), type(t), program_id(program)
{
    // std::cout << "ShaderProgram \"" << name << "\" (program id " << program_id << ") constructed"
    //           << std::endl;
}

ShaderProgram::~ShaderProgram()
{
    // std::cout << "ShaderProgram \"" << name << "\" (program id " << program_id
    //           << ") being cleaned up" << std::endl;
    glUseProgram(0);
    glDeleteProgram(program_id);
}

void ShaderProgram::use() const
{
    glUseProgram(program_id);
}

const uint ShaderProgram::getProgramId() const
{
    return program_id;
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

void ShaderProgram::setUniform3f(const std::string& name,
                                 const float f1,
                                 const float f2,
                                 const float f3) const
{
    glUniform3f(glGetUniformLocation(program_id, name.c_str()), f1, f2, f3);
}

void ShaderProgram::setUniform3fv(const std::string& name, const Eigen::Vector3f& vec) const
{
    glUniform3fv(glGetUniformLocation(program_id, name.c_str()), 1, vec.data());
}

void ShaderProgram::setUniformMatrix4fv(const std::string& name, const Eigen::Matrix4f& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, mat.data());
}

}  // namespace rendering
