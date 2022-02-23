#pragma once

#include <Eigen/Dense>

#include "rendering/shader_program.h"
#include "rendering/model.h"

namespace rendering
{
void draw_textured(const ShaderProgram& program,
                   const Mesh& mesh,
                   const Eigen::Isometry3f& pose,
                   const Texture& texture,
                   const int mode);

void draw_colored(const ShaderProgram& program,
                  const Mesh& mesh,
                  const Eigen::Isometry3f& pose,
                  const Eigen::Vector3f& color,
                  const int mode);

}  // namespace rendering
