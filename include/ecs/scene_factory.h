#pragma once

#include <memory>
#include <string>

#include "ecs/scene.h"

namespace ecs
{
struct SceneFactory
{
    static std::shared_ptr<Scene> create_from_scenario(const std::string& scenario_name);
};
}  // namespace ecs
