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
std::bitset<static_cast<int>(MotionControl::States::count)> MotionControl::get_bitset() const
{
    return bitset;
}

}  // namespace control
