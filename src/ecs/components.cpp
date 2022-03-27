#include "ecs/components.h"

geometry::MotionState
CameraComponent::get_target_state(const geometry::MotionState& tracked_entity_state,
                                  const Eigen::Isometry3f& relative_offset_pose)
{
    auto target_pose = tracked_entity_state.pose() * relative_offset_pose;
    geometry::MotionState target_state = tracked_entity_state;
    target_state.position = target_pose.translation();
    target_state.orientation = Eigen::Quaternionf(target_pose.linear());
    target_state.velocity *= 0.75f;
    target_state.acceleration *= 0.75f;
    target_state.velocity *= 0.75f;
    target_state.acceleration *= 0.75f;
    return target_state;
}

FighterComponent::FighterComponent(const std::string& name,
                                   entt::resource_handle<const urdf::FighterModel> model)
  : name(name), model(model)
{
}

bool FighterComponent::firing() const
{
    return input.test(urdf::FighterInput::Action::FIRE);
}

int FighterComponent::num_dispatches() const
{
    return static_cast<int>(model->fire_modes[current_fire_mode].type);
}

std::vector<FighterComponent::LaserDispatch> FighterComponent::fire_laser()
{
    auto out = std::vector<FighterComponent::LaserDispatch>();

    for (int i = 0; i < num_dispatches(); ++i)
    {
        auto spawn_pose = model->laser_spawn_poses[current_spawn_idx] *
                          Eigen::Translation<float, 3>(model->laser_info.size.x(), 0.0f, 0.0f);

        out.emplace_back(spawn_pose, model->laser_info);

        current_spawn_idx = (current_spawn_idx + 1) % model->laser_spawn_poses.size();
    }

    return out;
}

bool FighterComponent::laser_recharged(const float t)
{
    return t > last_fired_time + model->fire_modes[current_fire_mode].recharge_time;
}

std::optional<std::vector<FighterComponent::LaserDispatch>>
FighterComponent::try_fire_laser(const float t)
{
    if (laser_recharged(t))
    {
        last_fired_time = t;
        return fire_laser();
    }
    else
    {
        return std::nullopt;
    }
}

void FighterComponent::toggle_fire_mode()
{
    current_fire_mode = (current_fire_mode + 1) % model->fire_modes.size();
    input.set(urdf::FighterInput::Action::TOGGLE_FIRE_MODE, false);
}

void FighterComponent::try_toggle_fire_mode()
{
    if (input.test(urdf::FighterInput::Action::TOGGLE_FIRE_MODE))
    {
        toggle_fire_mode();
    }
}
