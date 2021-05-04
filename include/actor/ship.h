#pragma once

#include <string>
#include <optional>
#include <functional>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "actor/actor.h"
#include "control/fire_control.h"
#include "resources/description_data.h"

namespace actor
{
class Ship : public Actor
{
  public:
    Ship(const std::string& name,
         const resources::ActorDescription& description,
         const Eigen::Vector3f& position,
         const Eigen::Quaternionf& orientation,
         const std::string& geometry = "");

    static void
    set_on_fire_cb(std::function<void(const Ship& ship, const Eigen::Isometry3f relative_pose)> cb);

    virtual void tick(float current_time_s, float dt) override;

    bool is_firing;
    void toggle_fire_mode();

  private:
    static std::function<void(const Ship& ship, const Eigen::Isometry3f relative_pose)> on_fire_cb;
    control::FireControl fire_control;
};
}  // namespace actor