#pragma once

#include <string>

#include "shader_program.h"

namespace rendering
{
std::unique_ptr<ShaderProgram> compileComputeShader(const std::string& name,
                                                    const std::string& computeSource);

std::unique_ptr<ShaderProgram> compileShaders(const std::string& name,
                                              const std::string& vertex_filename,
                                              const std::string& fragment_filename);
}  // namespace rendering