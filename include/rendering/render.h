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
  - Draw sprite (spark, transparent, to FBO)
  - Draw spline (color)

*/

void render_model(const Model& model, const Eigen::Isometry3f& pose);
void render_skybox(const rendering::Texture& texture);
void render_spark(const Eigen::Isometry3f& pose, const Eigen::Vector3f& scale);
void render_screen_transition();
}  // namespace rendering