#pragma once

#include <vector>
#include <optional>
#include <entt/entt.hpp>
#include <Eigen/Dense>

#include "geometry/geometry.h"
#include "rendering/model.h"
#include "rendering/texture.h"
#include "urdf/fighter_input.h"
#include "urdf/fighter_model.h"

using MotionStateComponent = geometry::MotionState;

struct CameraComponent
{
    Eigen::Matrix4f perspective;
};

struct FighterComponent
{
    FighterComponent(const std::string& name, entt::resource_handle<const urdf::FighterModel> model)
      : name(name), model(model)
    {
    }

    std::string name;
    entt::resource_handle<const urdf::FighterModel> model;
    urdf::FighterInput input;
    int current_fire_mode = 0;
    int current_spawn_idx = 0;
    float last_fired_time = 0;

    using LaserDispatch = std::pair<Eigen::Isometry3f, urdf::FighterModel::LaserInfo>;

    bool firing() const;
    bool laser_recharged(const float t);
    int num_dispatches() const;
    std::vector<LaserDispatch> fire_laser();
    std::optional<std::vector<LaserDispatch>> try_fire_laser(const float t);
};

struct LaserComponent
{
    entt::entity producer;
};

struct SkyboxComponent
{
    entt::resource_handle<const rendering::Texture> texture;
    entt::resource_handle<const rendering::Model> model;
};

struct VisualComponent
{
    entt::resource_handle<const rendering::Model> model;
    std::optional<std::vector<entt::resource_handle<const rendering::Texture>>> textures;
    std::optional<Eigen::Vector3f> color = std::nullopt;
    std::optional<Eigen::Vector3f> size = std::nullopt;
};
