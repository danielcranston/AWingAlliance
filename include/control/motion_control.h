#pragma once

#include <iostream>
#include <bitset>

namespace control
{
class MotionControl
{
  public:
    enum class States
    {
        TURN_UP,
        TURN_DOWN,
        TURN_LEFT,
        TURN_RIGHT,
        ROLL_LEFT,
        ROLL_RIGHT,
        ACC_INCREASE,
        ACC_DECREASE,
        count
    };

    MotionControl();

    void turn_on(States s);
    void turn_off(States s);

    struct StateMap
    {
        bool test(States state) const
        {
            return states.test(static_cast<int>(state));
        }

        std::bitset<static_cast<int>(States::count)> states;
    };

    StateMap get_states() const;

    void set_states(const StateMap& states);

  private:
    StateMap state_map;
};
}  // namespace control
