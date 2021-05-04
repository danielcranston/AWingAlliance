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

    using StateMap = std::bitset<static_cast<int>(States::count)>;

    StateMap get_states() const;

    void set_states(const StateMap& states);

  private:
    StateMap bitset;
};
}  // namespace control
