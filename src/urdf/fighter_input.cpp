#include <unordered_map>
#include <SDL2/SDL.h>

#include "urdf/fighter_input.h"

namespace
{
const std::unordered_map<char, urdf::FighterInput::Action> key_action_map = {
    { static_cast<char>(SDLK_DOWN), urdf::FighterInput::Action::TURN_UP },
    { static_cast<char>(SDLK_UP), urdf::FighterInput::Action::TURN_DOWN },
    { static_cast<char>(SDLK_LEFT), urdf::FighterInput::Action::ROLL_LEFT },
    { static_cast<char>(SDLK_RIGHT), urdf::FighterInput::Action::ROLL_RIGHT },
    { static_cast<char>(SDLK_a), urdf::FighterInput::Action::TURN_LEFT },
    { static_cast<char>(SDLK_d), urdf::FighterInput::Action::TURN_RIGHT },
    { static_cast<char>(SDLK_w), urdf::FighterInput::Action::ACC_INCREASE },
    { static_cast<char>(SDLK_s), urdf::FighterInput::Action::ACC_DECREASE },
    { static_cast<char>(SDLK_SPACE), urdf::FighterInput::Action::FIRE },
    { static_cast<char>(SDLK_x), urdf::FighterInput::Action::TOGGLE_FIRE_MODE },
};
}  // namespace

namespace urdf
{
void FighterInput::handle_key_event(const KeyEvent& key_event)
{
    if (auto item = key_action_map.find(key_event.symbol); item != key_action_map.end())
    {
        if (key_event.status == KeyEvent::Status::PRESSED)
        {
            actions.set(static_cast<int>(item->second));
        }
        else
        {
            actions.reset(static_cast<int>(item->second));
        }
    }
}
}  // namespace urdf
