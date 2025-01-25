#pragma once

#include <string>
#include <Eigen/Dense>

namespace rendering
{

class ShaderProgram
{
  public:
    ShaderProgram(const std::string& uri,
                  const std::string& vertex_filename,
                  const std::string& fragment_filename);
    ~ShaderProgram();

    // Disallow copy constructor and copy assignment
    ShaderProgram(ShaderProgram&) = delete;
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    // Move constructor transfers ownership to avoid early deallocation of the shader. Relevant:
    // https://www.khronos.org/opengl/wiki/Common_Mistakes#RAII_and_hidden_destructor_calls
    // Const versions are possible thanks to is_owning being mutable. Relevant:
    // https://www.reddit.com/r/cpp/comments/8wbeom/coding_guideline_avoid_const_member_variables/
    ShaderProgram(ShaderProgram&&);
    ShaderProgram& operator=(ShaderProgram&&);

    // TODO: Implement if needed
    ShaderProgram(const ShaderProgram&&) = delete;
    ShaderProgram& operator=(const ShaderProgram&&) = delete;

    void set_uniform(const std::string& name, const int value) const;
    void set_uniform(const std::string& name, const float value) const;
    void set_uniform(const std::string& name, const float f1, const float f2) const;
    void set_uniform(const std::string& name, const Eigen::Vector3f& vec) const;
    void set_uniform(const std::string& name, const Eigen::Matrix4f& mat) const;
    void set_uniform(const std::string& name, const Eigen::Matrix<float, 4, 3>& mat) const;

    unsigned int program_id;
    std::string uri;

  private:
    mutable bool is_owning = true;
};

}  // namespace rendering
