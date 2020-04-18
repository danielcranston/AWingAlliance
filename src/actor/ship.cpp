#include <keyboard.h>
#include <actor/ship.h>

namespace actor
{
std::unique_ptr<Actor> Ship::Create(const glm::vec3& p, const glm::vec3& d, const Model* mdl)
{
    return std::unique_ptr<Actor>(new Ship(p, d, mdl));
}

Ship::Ship(const glm::vec3& p, const glm::vec3& d, const Model* mdl) : Actor(p, d, mdl)
{
}

void Ship::Update(const float dt)
{
    glm::mat3 rot = glm::rotate(glm::mat4(1.0f), dt * glm::pi<float>(), glm::vec3(1.0, 0.0, 0.0));
    dir = dir * rot;
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

// void Ship::Update(const std::bitset<8>& keyboardInfo, float dt)
// {
//     glm::vec3 rightVector = glm::cross(dir, glm::vec3(0.0, 1.0, 0.0));

//     // std::cout << "dt: " << dt << '\n';
//     if (keyboardInfo.test(KeyboardMapping::LEFTARROW))
//     {
//         dir = glm::vec3(glm::rotate(glm::mat4(1.0f), max_turnspeed * dt, glm::vec3(0, 1, 0)) *
//                         glm::vec4(dir, 1.0));
//     }
//     if (keyboardInfo.test(KeyboardMapping::RIGHTARROW))
//     {
//         dir = glm::vec3(glm::rotate(glm::mat4(1.0f), -max_turnspeed * dt, glm::vec3(0, 1, 0)) *
//                         glm::vec4(dir, 1.0));
//     }
//     if (keyboardInfo.test(KeyboardMapping::UPARROW) && dir.y > -0.9)
//     {
//         dir = glm::vec3(glm::rotate(glm::mat4(1.0f), -max_turnspeed * dt, rightVector) *
//                         glm::vec4(dir, 1.0));
//     }
//     if (keyboardInfo.test(KeyboardMapping::DOWNARROW) && dir.y < 0.9)
//     {
//         dir = glm::vec3(glm::rotate(glm::mat4(1.0f), max_turnspeed * dt, rightVector) *
//                         glm::vec4(dir, 1.0));
//     }

//     if (keyboardInfo.test(KeyboardMapping::W))
//         pos += glm::vec3(maxSpeed * dt * dir);
//     if (keyboardInfo.test(KeyboardMapping::S))
//         pos -= glm::vec3(maxSpeed * dt * dir);
//     if (keyboardInfo.test(KeyboardMapping::SPACEBAR))
//         pos = glm::vec3(0.0);
// }
}  // namespace actor
