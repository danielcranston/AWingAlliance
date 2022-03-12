#pragma once

#include <bitset>

#include "input/key_event.h"

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

    bool test(const Action action) const
    {
        return actions.test(static_cast<int>(action));
    }

    void handle_key_event(const KeyEvent& key_event);

  private:
    static constexpr int num_actions = static_cast<int>(Action::count);
    std::bitset<num_actions> actions;
};
}  // namespace urdf
