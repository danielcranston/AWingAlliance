#pragma once
#include <scene.h>

namespace ecs::systems
{
void render(const Scene& scene);
void integrate(Scene& scene, const float dt);
}  // namespace ecs::systems
