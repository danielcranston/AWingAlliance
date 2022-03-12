#pragma once

#include <vector>
#include <entt/entt.hpp>
#include <Eigen/Dense>

#include "geometry/geometry.h"
#include "urdf/fighter_input.h"
#include "urdf/fighter_model.h"

using MotionStateComponent = geometry::MotionState;

struct CameraComponent
{
    Eigen::Matrix4f perspective;
};

struct FighterComponent
{
    FighterComponent(entt::resource_handle<const urdf::FighterModel> model) : model(model)
    {
    }

    entt::resource_handle<const urdf::FighterModel> model;
    urdf::FighterInput input;
    int current_fire_mode = 0;
    int current_spawn_idx = 0;
    float last_fired_time = 0;

    bool firing() const
    {
        return input.test(urdf::FighterInput::Action::FIRE);
    }
};

struct SkyboxComponent
{
    entt::resource_handle<const rendering::Texture> texture;
    entt::resource_handle<const rendering::Model> model;
};

struct VisualComponent
{
    entt::resource_handle<const rendering::Model> model;
    std::vector<entt::resource_handle<const rendering::Texture>> textures;
};
