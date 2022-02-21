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
    struct FireMode
    {
        FireMode(int num_lasers, float recharge_time)
          : num_lasers(num_lasers), recharge_time(recharge_time)
        {
        }
        int num_lasers;
        float recharge_time;
    };

    std::vector<Eigen::Isometry3f> laser_spawn_points;
    std::vector<FireMode> fire_modes;
    int current_fire_mode;
    int current_spawn_idx;
    float last_fired_time;
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
