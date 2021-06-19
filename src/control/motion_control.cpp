#include "control/motion_control.h"

namespace control
{
MotionControl::MotionControl(){};

void MotionControl::turn_on(States s)
{
    state_map.states.set(static_cast<int>(s));
}

void MotionControl::turn_off(States s)
{
    state_map.states.reset(static_cast<int>(s));
}

MotionControl::StateMap MotionControl::get_states() const
{
    return state_map;
}

MotionControl::Actuation MotionControl::get_normalized_actuation() const
{
    static auto test = [this](MotionControl::States state1, MotionControl::States state2) {
        if (state_map.test(state1))
            return -1.0f;
        if (state_map.test(state2))
            return 1.0f;
        else
            return 0.0f;
    };

    Eigen::Vector3f d_w = {
        test(MotionControl::States::TURN_UP, MotionControl::States::TURN_DOWN),
        test(MotionControl::States::TURN_RIGHT, MotionControl::States::TURN_LEFT),
        test(MotionControl::States::ROLL_RIGHT, MotionControl::States::ROLL_LEFT)
    };

    float d_v = test(MotionControl::States::ACC_DECREASE, MotionControl::States::ACC_INCREASE);

    return { d_w, d_v };
}

void MotionControl::set_states(const MotionControl::StateMap& states)
{
    state_map = states;
}

}  // namespace control
