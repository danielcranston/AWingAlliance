#pragma once

#include <string>
#include <vector>
#include <array>
#include <limits>

namespace resources
{
struct ActorDescription
{
    struct FireControl
    {
        FireControl(std::vector<std::array<float, 3>> offsets,
                    std::vector<int> fire_modes,
                    std::vector<float> recharge_times_s)
          : offsets(std::move(offsets)),
            fire_modes(std::move(fire_modes)),
            recharge_times_s(std::move(recharge_times_s))
        {
        }
        std::vector<std::array<float, 3>> offsets;
        std::vector<int> fire_modes;
        std::vector<float> recharge_times_s;
    };

    ActorDescription(std::string name,
                     std::string type,
                     std::string visual,
                     FireControl primary_fire_control)
      : name(std::move(name)),
        type(std::move(type)),
        visual(std::move(visual)),
        primary_fire_control(std::move(primary_fire_control)){};
    std::string name;
    std::string type;
    std::string visual;
    FireControl primary_fire_control;
};
}  // namespace resources
