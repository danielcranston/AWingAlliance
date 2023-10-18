#pragma once

#include <vector>
#include <optional>
#include <entt/entt.hpp>
#include <Eigen/Dense>

#include "geometry/geometry.h"
#include "geometry/spline.h"
#include "rendering/model.h"
#include "rendering/texture.h"
#include "urdf/fighter_input.h"
#include "urdf/fighter_model.h"
#include "audio/audio.h"
#include "sm/roaming_state_machine.h"

using MotionStateComponent = geometry::MotionState;

struct RoamingStateMachineComponent
{
    RoamingStateMachineComponent(std::shared_ptr<sm::RoamingStateMachineContext> context)
      : context(context), instance(*context)
    {
        std::ignore = context;
    }

    std::shared_ptr<sm::RoamingStateMachineContext> context;
    sm::RoamingStateMachine::Instance instance;
};

struct CameraComponent
{
    Eigen::Matrix4f perspective;

    /**
     * @brief Creates a suitable target motion state to be fed to a camera controller.
     *
     * @param tracked_entity_state the MotionState of the entity being tracked
     * @param relative_offset_pose the camerass pose relative to the entity frame: T_entity_camera
     */
    geometry::MotionState get_target_state(const geometry::MotionState& tracked_entity_state,
                                           const Eigen::Isometry3f& relative_offset_pose);
};

struct FighterComponent
{
    FighterComponent(const std::string& name, entt::resource<const urdf::FighterModel> model);

    std::string name;
    entt::resource<const urdf::FighterModel> model;
    urdf::FighterInput input;
    int current_fire_mode = 0;
    int current_spawn_idx = 0;
    float last_fired_time = 0;
    std::optional<float> time_of_death = std::nullopt;

    using LaserDispatch = std::pair<Eigen::Isometry3f, urdf::FighterModel::LaserInfo>;

    bool alive() const;
    bool firing() const;
    bool laser_recharged(const float t);
    int num_dispatches() const;
    std::vector<LaserDispatch> fire_laser();
    std::optional<std::vector<LaserDispatch>> try_fire_laser(const float t);
    void toggle_fire_mode();
    void try_toggle_fire_mode();

    /**
     * @brief Creates a suitable target motion state to be fed to a ship controller, based on the
     * current motion and key input states.
     *
     * @param motion_state the MotionState related to this FighterComponent
     */
    geometry::MotionState get_target_state(const geometry::MotionState& motion_state) const;

    std::unique_ptr<audio::AudioSource> fire_sound_source;
    std::unique_ptr<audio::AudioSource> engine_sound_source;
};

struct LaserComponent
{
    entt::entity producer;
    entt::resource<const urdf::FighterModel> fighter_model;
    float length;
};

struct SkyboxComponent
{
    entt::resource<const rendering::Texture> texture;
    entt::resource<const rendering::Model> model;
};

struct VisualComponent
{
    entt::resource<const rendering::Model> model;
    std::optional<std::vector<entt::resource<const rendering::Texture>>> textures;
    std::optional<Eigen::Vector3f> color = std::nullopt;
    std::optional<Eigen::Vector3f> size = std::nullopt;
};

struct SoundEffectComponent
{
    entt::resource<const audio::AudioBuffer> buffer;
    std::unique_ptr<audio::AudioSource> sound_source;
};

struct BillboardComponent
{
    Eigen::Matrix4f size;
    float birth_time;
    float duration;
};

struct HealthComponent
{
    float shields;
    float hull;

    void take_damage(const float damage);
};

struct SplineComponent
{
    geometry::CubicBezierCurve curve;
};
