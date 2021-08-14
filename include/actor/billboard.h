#pragma once

#include "actor/actor.h"

namespace actor
{
class Billboard : public Actor
{
  public:
    Billboard(const std::string& name,
              const Eigen::Vector3f& position,
              const Eigen::Quaternionf& orientation,
              const Eigen::Vector3f& velocity,
              const Eigen::Vector2f& scale_vec,
              const float start_time,
              const float end_time);

    bool is_alive() const;
    float get_start_time() const;
    const Eigen::Matrix4f& get_scale() const;

    virtual void tick(float current_time_s, float dt) override;

  private:
    float start_time;
    float end_time;
    bool alive = true;
    Eigen::Matrix4f scale;
};
}  // namespace actor
