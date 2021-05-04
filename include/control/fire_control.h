#pragma once

#include <string>
#include <vector>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "resources/description_data.h"

namespace control
{
struct FireMode
{
    explicit FireMode(int start_index, int indices, float recharge_time_s);
    int start_index;
    int indices;
    float recharge_time_s;
};
class FireControl
{
  public:
    FireControl(const resources::ActorDescription::FireControl& f_control);
    FireControl(std::vector<Eigen::Vector3f> offsets,
                std::vector<int> fire_modes,
                std::vector<float> recharge_times_s);

    void toggle_fire_mode();

    int get_fire_mode() const;

    std::optional<std::vector<Eigen::Vector3f>> fire(float now_s);

  private:
    std::vector<Eigen::Vector3f> offsets;
    std::vector<FireMode> modes;
    int current_mode;
    int current_offset_index;
    float last_fire_time;
};
}  // namespace control
