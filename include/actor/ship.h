#pragma once

#include <string>
#include <optional>
#include <functional>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "actor/actor.h"
#include "control/fire_control.h"
#include "control/motion_model.h"
#include "control/motion_control.h"
#include "control/ship_controller.h"
#include "resources/description_data.h"

namespace actor
{
class Ship : public Actor
{
  public:
    struct InputStates
    {
        InputStates()
        {
            for (auto& state : motion_control_states)
            {
                state = std::nullopt;
            }
        }

        std::array<std::optional<bool>, static_cast<int>(control::MotionControl::States::count)>
            motion_control_states;
        bool changed_fire_mode = false;
        std::optional<bool> is_firing = std::nullopt;
    };

    Ship(const std::string& name,
         const resources::ActorDescription& description,
         const Eigen::Vector3f& position,
         const Eigen::Quaternionf& orientation,
         const std::string& geometry = "");
    ~Ship() = default;

    static void
    set_on_fire_cb(std::function<void(const Ship& ship, const Eigen::Isometry3f relative_pose)> cb);

    virtual void tick(float current_time_s, float dt) override;

    bool is_firing;
    void toggle_fire_mode();

    void update_input_states(const InputStates& req);

  private:
    static std::function<void(const Ship& ship, const Eigen::Isometry3f relative_pose)> on_fire_cb;
    control::FireControl fire_control;

    control::ShipController ship_controller;
    control::MotionControl motion_control;
    control::MotionModel motion_model;
};
}  // namespace actor