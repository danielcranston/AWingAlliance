#pragma once

#include <vector>
#include <Eigen/Geometry>
#include "geometry/geometry.h"

namespace urdf
{
struct FighterModel
{
    std::string visual_name;
    Eigen::Vector3f dimensions;

    std::vector<Eigen::Isometry3f> laser_spawn_poses;
    std::vector<Eigen::Isometry3f> camera_poses;
    std::vector<Eigen::Isometry3f> exhaust_poses;

    struct LaserInfo
    {
        float speed;
        Eigen::Vector3f color;
        Eigen::Vector3f size;

        struct ImpactInfo
        {
            Eigen::Vector3f size;
            float duration;
        };
        ImpactInfo impact_info;
    };
    LaserInfo laser_info;

    struct Sounds
    {
        std::string laser;
        std::string engine;
        std::string hit;
    };
    Sounds sounds;

    struct FireMode
    {
        enum class FireType
        {
            SINGLE = 1,
            DUAL = 2
        };
        FireType type;
        float recharge_time;
    };
    std::vector<FireMode> fire_modes;

    struct MotionLimits
    {
        float velocity = 1;
        float acceleration = 1;
        float angular_velocity = 1;
        float angular_acceleration = 1;
    };
    MotionLimits motion_limits;

    struct HealthInfo
    {
        float shields_max;
        float hull_max;
    };
    HealthInfo health_info;

    void apply_motion_limits(geometry::MotionState& state) const
    {
        auto apply_limit = [](Eigen::Vector3f& x, const float limit) {
            if (auto norm = x.norm(); std::abs(norm) > 10e-8)
            {
                x = std::min(norm, limit) * (x / norm);
            }
        };

        apply_limit(state.velocity, motion_limits.velocity);
        apply_limit(state.acceleration, motion_limits.acceleration);
        apply_limit(state.angular_velocity, motion_limits.angular_velocity);
        apply_limit(state.angular_acceleration, motion_limits.angular_acceleration);
    }
};
}  // namespace urdf
