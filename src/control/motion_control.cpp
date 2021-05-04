#include "control/motion_control.h"

namespace control
{
MotionControl::MotionControl(){};

void MotionControl::turn_on(States s)
{
    bitset.set(static_cast<int>(s));
}

void MotionControl::turn_off(States s)
{
    bitset.reset(static_cast<int>(s));
}

MotionControl::StateMap MotionControl::get_states() const
{
    return bitset;
}

void MotionControl::set_states(const MotionControl::StateMap& states)
{
    bitset = states;
}

}  // namespace control