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
    set_position(position);
    set_orientation(orientation);
}

Actor::~Actor(){};

void Actor::tick(float current_time_s, float dt)
{
}

void Actor::set_position(const Eigen::Vector3f& pos)
{
    motion_state.position = pos;
}

void Actor::set_orientation(const Eigen::Quaternionf& quat)
{
    motion_state.orientation = quat;
}

void Actor::set_pose(const Eigen::Isometry3f& p)
{
    set_position(p.translation());
    set_orientation(Eigen::Quaternionf(p.linear()));
}

void Actor::set_target_pose(const Eigen::Isometry3f& target)
{
    target_pose = target;
}

const Eigen::Vector3f Actor::get_position() const
{
    return motion_state.position;
}

const Eigen::Quaternionf Actor::get_orientation() const
{
    return motion_state.orientation;
}

const Eigen::Isometry3f Actor::get_pose() const
{
    return geometry::make_pose(motion_state.position, motion_state.orientation);
}

const Eigen::Isometry3f Actor::get_target_pose() const
{
    return target_pose;
}

std::string Actor::get_name() const
{
    return name;
}

Eigen::Vector3f Actor::get_right_dir() const
{
    return geometry::get_right_dir(motion_state.orientation.toRotationMatrix());
}

Eigen::Vector3f Actor::get_up_dir() const
{
    return geometry::get_up_dir(motion_state.orientation.toRotationMatrix());
}

Eigen::Vector3f Actor::get_fwd_dir() const
{
    return geometry::get_fwd_dir(motion_state.orientation.toRotationMatrix());
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