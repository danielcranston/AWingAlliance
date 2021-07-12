#pragma once

#include <string>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "geometry.h"

namespace actor
{
class Actor
{
  public:
    Actor(const std::string& name,
          const Eigen::Vector3f& position,
          const Eigen::Quaternionf& orientation,
          const std::string& visual = "",
          const std::string& geometry = "");
    virtual ~Actor();

    virtual void tick(float current_time_s, float dt);

    void set_position(const Eigen::Vector3f& pos);
    void set_orientation(const Eigen::Quaternionf& quat);
    void set_pose(const Eigen::Isometry3f& pose);
    void set_target_pose(const Eigen::Isometry3f& target_pose);

    const Eigen::Vector3f get_position() const;
    const Eigen::Quaternionf get_orientation() const;
    const Eigen::Isometry3f get_pose() const;
    const Eigen::Isometry3f get_target_pose() const;
    const geometry::MotionState get_motion_state() const;

    std::string get_name() const;

    Eigen::Vector3f get_right_dir() const;
    Eigen::Vector3f get_up_dir() const;
    Eigen::Vector3f get_fwd_dir() const;

    bool has_visual() const;
    const std::string& get_visual_name() const;

    bool has_geometry() const;
    const std::string& get_geometry_name() const;

    bool operator==(const Actor& other) const;

  protected:
    std::string name;
    std::string visual = "";
    std::string geometry = "";
    mutable geometry::MotionState motion_state;
    Eigen::Isometry3f target_pose = Eigen::Isometry3f::Identity();
};
}  // namespace actor
