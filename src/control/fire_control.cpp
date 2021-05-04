#include <iostream>

#include "control/fire_control.h"

namespace control
{
FireMode::FireMode(int start_index, int indices, float recharge_time_s)
  : start_index(start_index), indices(indices), recharge_time_s(recharge_time_s)
{
}

FireControl::FireControl(const resources::ActorDescription::FireControl& f_control)
{
    offsets.reserve(f_control.offsets.size());
    std::transform(f_control.offsets.begin(),
                   f_control.offsets.end(),
                   std::back_insert_iterator(offsets),
                   [](std::array<float, 3> offset) {
                       return Eigen::Vector3f(offset[0], offset[1], offset[2]);
                   });

    modes.reserve(f_control.fire_modes.size());
    for (int i = 0; i < f_control.fire_modes.size(); ++i)
    {
        modes.emplace_back(0, f_control.fire_modes[i], f_control.recharge_times_s[i]);
    }

    current_mode = 0;
    current_offset_index = 0;
    last_fire_time = 0.0f;
}

std::optional<std::vector<Eigen::Vector3f>> FireControl::fire(float now_s)
{
    if (now_s >= last_fire_time + modes[current_mode].recharge_time_s)
    {
        last_fire_time = now_s;

        std::vector<Eigen::Vector3f> dispatches;
        for (int i = 0; i < modes[current_mode].indices; ++i)
        {
            dispatches.emplace_back(offsets[current_offset_index]);

            current_offset_index = (current_offset_index + 1) % offsets.size();
        }
        return dispatches;
    }
    else
    {
        return std::nullopt;
    }
}

void FireControl::toggle_fire_mode()
{
    current_mode = (current_mode + 1) % modes.size();
    current_offset_index = modes[current_mode].start_index;
}

int FireControl::get_fire_mode() const
{
    return current_mode;
}

}  // namespace control
