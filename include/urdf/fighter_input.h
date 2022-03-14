#pragma once

#include <bitset>

#include "input/key_event.h"
#include "geometry/geometry.h"

namespace urdf
{
class FighterInput
{
  public:
    enum class Action
    {
        TURN_UP,
        TURN_DOWN,
        TURN_LEFT,
        TURN_RIGHT,
        ROLL_LEFT,
        ROLL_RIGHT,
        ACC_INCREASE,
        ACC_DECREASE,
        FIRE,
        TOGGLE_FIRE_MODE,
        count
    };

    bool test(const Action action) const;

    void handle_key_event(const KeyEvent& key_event);

    struct Actuation
    {
        Eigen::Vector3f d_w;  // normalized instantaneous angular velocity in body frame
        float d_v;            // normalized instantaneous linear velocity in body frame

        Eigen::Quaternionf d_q(float dt)
        {
            return geometry::angular_velocity_to_quat(d_w, dt);
        }
    };

    Actuation current_actuation() const;

  private:
    static constexpr int num_actions = static_cast<int>(Action::count);
    std::bitset<num_actions> actions;
};
}  // namespace urdf
