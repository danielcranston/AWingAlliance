#include <keyboard.h>
#include "actor/ship.h"
#include "geometry.h"
#include <chrono>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_cross_product.hpp>
#include <glm/common.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

namespace actor
{
std::unique_ptr<Ship> Ship::Create(const glm::vec3& p,
                                   const glm::vec3& d,
                                   const std::string& name,
                                   const Model* mdl,
                                   const Team team)
{
    return std::unique_ptr<Ship>(new Ship(p, d, name, mdl, team));
}

Ship::Ship(const glm::vec3& p,
           const glm::vec3& d,
           const std::string& name,
           const Model* mdl,
           const Team team)
  : Actor(p, d, mdl),
    name(name),
    desired_dir(d),
    team(team),
    controller(this),
    last_fired_time(std::chrono::system_clock::now())
{
}

void Ship::Update(const float dt)
{
    controller.Tick();
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

void Ship::TakeDamage(const int amount)
{
    health -= amount;
}

const int Ship::GetHealth() const
{
    return health;
}

const Team& Ship::GetTeam() const
{
    return team;
}

const float Ship::GetSpeed() const
{
    return max_speed;
}

const std::string& Ship::GetName() const
{
    return name;
}

void Ship::Follow(const Actor& target, const float dt)
{
    MoveToLocation(target.GetPosition(), dt);
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
    glm::quat quat = geometry::RotationBetweenVectors(dir, target_pos - pos);
    glm::mat3 R(quat);
    dir = glm::normalize((50 * dt * quat) * dir);

    float closest_distance = 20.0f;
    float thresh_distance = 40.0f;

    float distance = std::abs(glm::l2Norm(pos - target_pos));
    float dist_multiplier = std::min(std::max(0.0f, distance - 20.0f), 20.0f);

    dist_multiplier = glm::smoothstep(0.0f, 20.0f, dist_multiplier);

    pos = pos + dir * max_speed * dist_multiplier * dt;  // std::min(max_speed, distance);
}
void Ship::Update(const std::bitset<8>& keyboardInfo, float dt)
{
    if (keyboardInfo.test(KeyboardMapping::LEFTARROW))
    {
        roll = std::max(roll - 0.001f * glm::pi<float>(), -0.15f * glm::pi<float>());

        desired_dir =
            glm::vec3(glm::rotate(glm::mat4(1.0f), max_turnspeed * dt, glm::vec3(0, 1, 0)) *
                      glm::vec4(desired_dir, 1.0));
    }
    if (keyboardInfo.test(KeyboardMapping::RIGHTARROW))
    {
        roll = std::min(roll + 0.001f * glm::pi<float>(), +0.15f * glm::pi<float>());
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

    if (!keyboardInfo.test(KeyboardMapping::RIGHTARROW) &&
        !keyboardInfo.test(KeyboardMapping::LEFTARROW))
        roll -= 0.003f * roll * glm::pi<float>();

    glm::quat quat = geometry::RotationBetweenVectors(dir, desired_dir);
    dir = glm::normalize((0.1f * quat) * dir);

    if (keyboardInfo.test(KeyboardMapping::W))
        pos += glm::vec3(max_speed * dt * dir);
    if (keyboardInfo.test(KeyboardMapping::S))
        pos -= glm::vec3(max_speed * dt * dir);
    if (keyboardInfo.test(KeyboardMapping::SPACEBAR))
        Fire();
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

std::function<void(const Laser& laser)> Ship::RegisterLaserFunc = [](const Laser& laser) {
    throw std::runtime_error("You need to set define your own function to register lasers");
};

}  // namespace actor
