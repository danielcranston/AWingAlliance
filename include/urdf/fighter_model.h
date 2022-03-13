#pragma once

#include <vector>
#include <Eigen/Geometry>

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
};
}  // namespace urdf
