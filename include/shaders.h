#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>

// enum class UniformType
// {
//     Uniform1i,
//     Uniform2f,
//     Uniform3f,
//     Uniform3fv,
//     UniformMatrix4fv

// };

// template <typename T>
// struct Uniform
// {
//     explicit Uniform(const std::string& n, const UniformType t, const std::vector<T> args);
//     std::string name;
//     UniformType type;
//     std::vector<T> arguments;
// };

enum class ShaderType
{
    NORMAL,
    COMPUTE
};

class ShaderProgram
{
  public:
    explicit ShaderProgram(const std::string& n, const ShaderType t, const uint program);

    void Use() const;
    const uint GetProgram() const;
    void SetUniform1i(const std::string& name, const int value) const;
    void SetUniform1f(const std::string& name, const float value) const;
    void SetUniform2f(const std::string& name, const float f1, const float f2) const;
    void
    SetUniform3f(const std::string& name, const float f1, const float f2, const float f3) const;
    void SetUniform3fv(const std::string& name, const glm::vec3& vec) const;
    void SetUniformMatrix4fv(const std::string& name, const glm::mat4& mat) const;

  private:
    const std::string name;
    ShaderType type;
    const uint program_id;
};

std::unique_ptr<ShaderProgram> compileShaders(const std::string& name,
                                              const std::string& vertexSource,
                                              const std::string& fragmentSource);
std::unique_ptr<ShaderProgram> compileComputeShader(const std::string& name,
                                                    const std::string& computeSource);
