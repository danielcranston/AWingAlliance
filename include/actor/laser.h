#pragma once

#include <list>
#include <array>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace actor
{
struct Laser
{
    explicit Laser(const glm::vec3& p,
                   const glm::vec3& d,
                   const std::chrono::system_clock::time_point expire_time);
    void Update(const float dt);

    const glm::vec3& GetPosition() const;
    const glm::vec3& GetDirection() const;
    const glm::mat4 GetPose() const;

    // In absence of a better place to put this
    static void RegisterLaser(std::list<Laser>& laser_vec, const Laser& new_laser);

    static constexpr float LENGTH = 4.0f;
    static const std::array<glm::vec3, 2> contact_points;

    bool alive = true;
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 color = { 1.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 0.2f, 0.2f, LENGTH };
    std::chrono::system_clock::time_point expire_time;
    const float speed = 250.0f;
};
}  // namespace actor
