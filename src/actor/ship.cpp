#include <keyboard.h>
#include "actor/ship.h"
#include <chrono>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_cross_product.hpp>
#include <glm/common.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

namespace
{
glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
{
    using namespace glm;
    start = normalize(start);
    dest = normalize(dest);

    float cosTheta = dot(start, dest);
    vec3 rotationAxis;

    if (cosTheta < -1 + 0.001f)
    {
        // special case when vectors in opposite directions
        return quat(0.0f, 0.0f, 0.0f, 1.0f);
    }

    rotationAxis = cross(start, dest);

    float s = sqrt((1 + cosTheta) * 2);
    float invs = 1 / s;

    return quat(s * 0.5f, rotationAxis.x * invs, rotationAxis.y * invs, rotationAxis.z * invs);
}
}  // namespace

namespace actor
{
std::unique_ptr<Ship> Ship::Create(const glm::vec3& p,
                                   const glm::vec3& d,
                                   const Model* mdl,
                                   const Team team,
                                   std::function<void(const Laser& Laser)> laser_func)
{
    return std::unique_ptr<Ship>(new Ship(p, d, mdl, team, laser_func));
}

Ship::Ship(const glm::vec3& p,
           const glm::vec3& d,
           const Model* mdl,
           const Team team,
           std::function<void(const Laser& Laser)> laser_func)
  : Actor(p, d, mdl),
    desired_dir(d),
    RegisterLaserFunc(laser_func),
    team(team),
    last_fired_time(std::chrono::system_clock::now())
{
}

void Ship::Update(const float dt)
{
    glm::mat3 rot = glm::rotate(glm::mat4(1.0f), dt * glm::pi<float>(), glm::vec3(1.0, 0.0, 0.0));
    dir = dir * rot;
}

void Ship::SetDesiredDir(const glm::vec3& new_dir)
{
    desired_dir = new_dir;
}

const glm::vec3& Ship::GetDesiredDir() const
{
    return desired_dir;
}

const glm::vec3& Ship::GetColor() const
{
    return color;
}

// void Ship::Update_Roaming(float t)
// {
//     if (t - s.start_time > s.duration)
//     {
//         s.UpdateRandom(pos, dir);
//         s.UploadPoints();
//     }

//     float u = (t - s.start_time) / s.duration;  // [0, 1]
//     std::pair<glm::vec3, glm::vec3> interp = s(u);
//     pos = interp.first;
//     dir = glm::normalize(interp.second);
// }

void Ship::Follow(const Actor& target, const float dt)
{
    MoveToLocation(target.GetPosition(), dt);
}

void Ship::Follow(const float dt)
{
    if (target)
    {
        Follow(*target, dt);
    }
}

bool Ship::IsInRange(const glm::vec3& target_pos, const float distance)
{
    return glm::l2Norm(pos - target_pos) < distance;
}

bool Ship::IsColliding(const Ship& other)
{
    glm::mat4 pose_relative_other = glm::inverse(other.GetPose()) * GetPose();
    return other.model->GetBoundingBox().is_inside(model->GetBoundingBox(), pose_relative_other);
}

bool Ship::IsColliding(const Laser& laser)
{
    glm::mat4 pose_relative_other = glm::inverse(GetPose()) * laser.GetPose();
    for (const auto& point : Laser::contact_points)
    {
        glm::vec3 transformed = pose_relative_other * glm::vec4(point, 1.0f);
        if (model->GetBoundingBox().is_inside(transformed))
            return true;
    }
    return false;
}
void Ship::MoveToLocation(glm::vec3 target_pos, const float dt)
{
    glm::quat quat = RotationBetweenVectors(dir, target_pos - pos);
    glm::mat3 R(quat);
    dir = glm::normalize((0.15f * quat) * dir);

    float closest_distance = 20.0f;
    float thresh_distance = 40.0f;

    float distance = std::abs(glm::l2Norm(pos - target_pos));
    float dist_multiplier = std::min(std::max(0.0f, distance - 20.0f), 20.0f);

    dist_multiplier = glm::smoothstep(0.0f, 20.0f, dist_multiplier);

    pos = pos + dir * max_speed * dist_multiplier * dt;  // std::min(max_speed, distance);
}

void Ship::SetTarget(Ship* const new_target)
{
    target = new_target;
}

void Ship::ClearTarget()
{
    target = nullptr;
}

void Ship::Update(const std::bitset<8>& keyboardInfo, float dt)
{
    // std::cout << "dt: " << dt << '\n';
    if (keyboardInfo.test(KeyboardMapping::LEFTARROW))
    {
        desired_dir =
            glm::vec3(glm::rotate(glm::mat4(1.0f), max_turnspeed * dt, glm::vec3(0, 1, 0)) *
                      glm::vec4(desired_dir, 1.0));
    }
    if (keyboardInfo.test(KeyboardMapping::RIGHTARROW))
    {
        desired_dir =
            glm::vec3(glm::rotate(glm::mat4(1.0f), -max_turnspeed * dt, glm::vec3(0, 1, 0)) *
                      glm::vec4(desired_dir, 1.0));
    }
    glm::vec3 rightVector = glm::cross(dir, glm::vec3(0.0, 1.0, 0.0));
    if (keyboardInfo.test(KeyboardMapping::UPARROW) && desired_dir.y > -0.9)
    {
        desired_dir = glm::vec3(glm::rotate(glm::mat4(1.0f), -max_turnspeed * dt, rightVector) *
                                glm::vec4(desired_dir, 1.0));
    }
    if (keyboardInfo.test(KeyboardMapping::DOWNARROW) && desired_dir.y < 0.9)
    {
        desired_dir = glm::vec3(glm::rotate(glm::mat4(1.0f), max_turnspeed * dt, rightVector) *
                                glm::vec4(desired_dir, 1.0));
    }

    glm::quat quat = RotationBetweenVectors(dir, desired_dir);
    dir = glm::normalize((0.25f * quat) * dir);

    if (keyboardInfo.test(KeyboardMapping::W))
        pos += glm::vec3(max_speed * dt * dir);
    if (keyboardInfo.test(KeyboardMapping::S))
        pos -= glm::vec3(max_speed * dt * dir);
}

void Ship::Fire()
{
    using namespace std::chrono;
    const auto seconds_since_last_fired =
        duration_cast<milliseconds>(system_clock::now() - last_fired_time);

    if (seconds_since_last_fired > fire_recharge_time)
    {
        glm::vec3 laser_dir = GetDirection();
        glm::vec3 laser_pos = GetPosition();
        laser_pos += laser_dir * (Laser::LENGTH + 2.0f * model->GetBoundingBox().scale.z);
        const Laser laser{ laser_pos, laser_dir, system_clock::now() + seconds(1) };
        RegisterLaserFunc(laser);

        last_fired_time = system_clock::now();
    }
}

}  // namespace actor
