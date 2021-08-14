#pragma once

#include "actor/actor.h"

namespace actor
{
class Laser : public Actor
{
  public:
    Laser(const std::string& name,
          const Eigen::Vector3f& position,
          const Eigen::Quaternionf& orientation,
          const Eigen::Vector3f& scale_vec,
          const Eigen::Vector3f& color,
          const float speed,
          const std::string& visual = "",
          const std::string& geometry = "");

    virtual void tick(float current_time_s, float dt) override;

    const Eigen::Vector3f& get_color();
    const Eigen::Matrix4f& get_scale();

  private:
    bool alive;
    Eigen::Vector3f color;
    Eigen::Matrix4f scale;
};
}  // namespace actor
