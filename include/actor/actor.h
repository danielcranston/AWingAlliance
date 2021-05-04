#pragma once

#include <string>

#include <Eigen/Dense>
#include <Eigen/Geometry>

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

    const Eigen::Vector3f get_position() const;
    const Eigen::Quaternionf get_orientation() const;
    const Eigen::Isometry3f& get_pose() const;

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
    Eigen::Isometry3f pose;
    std::string visual = "";
    std::string geometry = "";
};
}  // namespace actor
