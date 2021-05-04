#include "actor/actor.h"

#include "geometry.h"

namespace actor
{
Actor::Actor(const std::string& name,
             const Eigen::Vector3f& position,
             const Eigen::Quaternionf& orientation,
             const std::string& visual,
             const std::string& geometry)
  : name(name), visual(visual), geometry(geometry)
{
    pose = Eigen::Isometry3f(orientation.matrix());
    pose.translation() = position;
}

Actor::~Actor(){};

void Actor::tick(float current_time_s, float dt)
{
}

void Actor::set_position(const Eigen::Vector3f& pos)
{
    pose.translation() = pos;
}

void Actor::set_orientation(const Eigen::Quaternionf& quat)
{
    pose.linear() = quat.matrix();
}

void Actor::set_pose(const Eigen::Isometry3f& p)
{
    pose.translation() = p.translation();
    pose.linear() = p.linear();
}

const Eigen::Vector3f Actor::get_position() const
{
    return pose.translation();
}

const Eigen::Quaternionf Actor::get_orientation() const
{
    return Eigen::Quaternionf(pose.linear());  // TODO: surely there are better
                                               // conversions
}

const Eigen::Isometry3f& Actor::get_pose() const
{
    return pose;
}

std::string Actor::get_name() const
{
    return name;
}

Eigen::Vector3f Actor::get_right_dir() const
{
    return geometry::get_right_dir(pose.matrix());
}

Eigen::Vector3f Actor::get_up_dir() const
{
    return geometry::get_up_dir(pose.matrix());
}

Eigen::Vector3f Actor::get_fwd_dir() const
{
    return geometry::get_fwd_dir(pose.matrix());
}

bool Actor::has_visual() const
{
    return visual != "";
}

const std::string& Actor::get_visual_name() const
{
    assert(has_visual());

    return visual;
}

bool Actor::has_geometry() const
{
}

const std::string& Actor::get_geometry_name() const
{
}

bool Actor::operator==(const Actor& other) const
{
    return this == &other;
}

}  // namespace actor