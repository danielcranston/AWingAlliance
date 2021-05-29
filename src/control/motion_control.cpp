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

void MotionControl::set_states(const MotionControl::StateMap& states)
{
    state_map = states;
}

}  // namespace control
