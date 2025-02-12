#pragma once

#include <optional>
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

struct RenderOptions
{
    enum class DrawMode
    {
        TRIANGLES,
        LINE_STRIP
    };
    DrawMode draw_mode = DrawMode::TRIANGLES;
    std::optional<Eigen::Vector3f> scale;
    std::optional<Eigen::Vector3f> color;
    std::optional<float> alpha;
    std::optional<std::string> custom_shader_uri;
    Texture* custom_texture = nullptr;
};

void render_quad(const Eigen::Isometry3f& pose,
                 const std::optional<RenderOptions>& options = std::nullopt);
void render_model(const Model& model,
                  const Eigen::Isometry3f& pose,
                  const std::optional<RenderOptions>& options = std::nullopt);
void render_skybox(const rendering::Texture& texture);
void render_fullscreen(const float start_time,
                       const std::optional<RenderOptions>& options = std::nullopt);
}  // namespace rendering
