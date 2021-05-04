#pragma once

#include <map>
#include <optional>

#include <Eigen/Dense>

#include "rendering/shader_program.h"
#include "rendering/model.h"

namespace rendering
{
void draw(const ShaderProgram& program,
          const Model& model,
          const Eigen::Isometry3f& pose,
          const Eigen::Vector3f& color,
          const std::map<std::string, rendering::Texture>& textures_map,
          const int mode);

void draw(const ShaderProgram& program,
          const Model& model,
          const Eigen::Isometry3f& pose,
          const Eigen::Vector3f& color,
          const std::optional<std::reference_wrapper<const Texture>> texture,
          const int mode);

}  // namespace rendering
