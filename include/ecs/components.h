#pragma once

#include <vector>
#include <entt/entt.hpp>
#include <Eigen/Dense>

#include "geometry/geometry.h"

using MotionStateComponent = geometry::MotionState;

struct CameraComponent
{
    Eigen::Matrix4f perspective;
};

struct FighterComponent
{
    // entt:resource_handle<const urdf::FigherModel> model;
    int current_fire_mode;
    int current_spawn_idx;
    float last_fired_time;
    bool firing;
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
