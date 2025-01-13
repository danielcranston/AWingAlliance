#pragma once

#include <string>
#include <memory>
#include <Eigen/Dense>

namespace rendering
{

struct ShaderProgram
{
    ShaderProgram(const std::string& uri,
                  const std::string& vertex_filename,
                  const std::string& fragment_filename);
    ~ShaderProgram();

    // Disallow copy constructor and copy assignment
    ShaderProgram(ShaderProgram&) = delete;
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    void use() const;

    void setUniform1i(const std::string& name, const int value) const;
    void setUniform1f(const std::string& name, const float value) const;
    void setUniform2f(const std::string& name, const float f1, const float f2) const;
    void setUniform3fv(const std::string& name, const Eigen::Vector3f& vec) const;
    void setUniformMatrix4fv(const std::string& name, const Eigen::Matrix4f& mat) const;
    // Note: OpenGL nxm convention is backwards, see
    // https://www.khronos.org/opengl/wiki/Data_Type_(GLSL)#Matrices
    void setUniformMatrix3x4fv(const std::string& name,
                               const Eigen::Matrix<float, 4, 3>& mat) const;

    unsigned int program_id;
    std::string uri;
};

}  // namespace rendering
