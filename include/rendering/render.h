#pragma once

#include <vector>

#include "rendering/model.h"
#include "rendering/shader_program.h"
#include "rendering/texture.h"

namespace rendering
{

/*
Use cases:
  - Draw model (textured, colored, with size)
  - Draw model (color)
  - Draw skybox (textured)
  - Draw sprite (hyperspace)
  - Draw spline (color, line strip)

*/

void render_model(const Model& model,
                  const Eigen::Isometry3f& pose,
                  const Eigen::Vector3f& scale = Eigen::Vector3f::Ones());
void render_skybox(const rendering::Texture& texture);
void render_spark(const Eigen::Isometry3f& pose,
                  const Eigen::Vector3f& scale,
                  const float start_time);
void render_screen_transition();
}  // namespace rendering