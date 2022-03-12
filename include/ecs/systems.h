#pragma once
#include "ecs/scene.h"
#include "input/key_event.h"
namespace ecs::systems
{
void render(const Scene& scene);
void integrate(Scene& scene, const float dt);
void handle_key_events(Scene& scene, const std::vector<KeyEvent>& key_events);
}  // namespace ecs::systems
