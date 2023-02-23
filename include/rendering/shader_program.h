#pragma once

#include <string>
#include <memory>
#include <Eigen/Dense>

namespace rendering
{
enum class ShaderType
{
    NORMAL,
    COMPUTE
};

class ShaderProgram
{
  public:
    explicit ShaderProgram(const std::string& n, const ShaderType t, const uint program);
    virtual ~ShaderProgram();

    void use() const;
    uint getProgramId() const;
    void setUniform1i(const std::string& name, const int value) const;
    void setUniform1f(const std::string& name, const float value) const;
    void setUniform2f(const std::string& name, const float f1, const float f2) const;
    void
    setUniform3f(const std::string& name, const float f1, const float f2, const float f3) const;
    void setUniform3fv(const std::string& name, const Eigen::Vector3f& vec) const;
    void setUniformMatrix4fv(const std::string& name, const Eigen::Matrix4f& mat) const;
    // Note: OpenGL nxm convention is backwards, see
    // https://www.khronos.org/opengl/wiki/Data_Type_(GLSL)#Matrices
    void setUniformMatrix3x4fv(const std::string& name,
                               const Eigen::Matrix<float, 4, 3>& mat) const;

  private:
    const std::string name;
    const ShaderType type;
    const uint program_id;
};

}  // namespace rendering
