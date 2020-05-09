#include "actor/laser.h"

const std::array<glm::vec3, 2> Laser::contact_points = {
    glm::vec3(0.0f, 0.0f, Laser::LENGTH / 2.0f),
    glm::vec3(0.0f, 0.0f, -Laser::LENGTH / 2.0f)
};

Laser::Laser(const glm::vec3& p,
             const glm::vec3& d,
             const std::chrono::system_clock::time_point expire_time)
  : pos(p), dir(d), expire_time(expire_time)
{
}

void Laser::Update(const float dt)
{
    pos += glm::vec3(dt * speed * dir);
}

const glm::mat4 Laser::GetPose() const
{
    glm::mat4 rot = glm::lookAt(glm::vec3(0.0, 0.0, 0.0), -dir, glm::vec3(0.0, 1.0, 0.0));
    return glm::translate(glm::mat4(1.0f), pos) * glm::transpose(rot) *
           glm::scale(glm::mat4(1.0f), scale);
}

void Laser::RegisterLaser(std::list<Laser>& laser_vec, const Laser& new_laser)
{
    laser_vec.push_back(new_laser);
}
