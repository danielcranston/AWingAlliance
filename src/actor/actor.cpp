#include <iostream>
#include <memory>
#include <bitset>
#include <map>

#include <actor/actor.h>
#include <GL/glew.h>

namespace actor
{
std::unique_ptr<Actor> Actor::Create(const glm::vec3& p, const glm::vec3& d, const Model* mdl)
{
    return std::unique_ptr<Actor>(new Actor(p, d, mdl));
}

Actor::Actor(const glm::vec3& p, const glm::vec3& d, const Model* mdl)
  : pos{ p }, dir{ glm::normalize(d) }, model{ mdl }
{
}

void Actor::SetPosition(const glm::vec3& p)
{
    pos = p;
}

void Actor::SetDirection(const glm::vec3& d)
{
    dir = d;
}

void Actor::SetPose(const glm::vec3& p, const glm::vec3& d)
{
    pos = p;
    dir = d;
}

const glm::vec3& Actor::GetPosition() const
{
    return pos;
}

const glm::vec3& Actor::GetDirection() const
{
    return dir;
}

const glm::mat4 Actor::GetPose() const
{
    glm::mat4 rot = glm::lookAt(glm::vec3(0.0, 0.0, 0.0), -dir, glm::vec3(0.0, 1.0, 0.0));
    return glm::translate(glm::mat4(1.0f), pos) * glm::transpose(rot);
}

const Model* Actor::GetModel() const
{
    return model;
}

const glm::vec3& Actor::GetColor() const
{
    return color;
}

void Actor::Update(const float dt)
{
    glm::mat3 rot = glm::rotate(glm::mat4(1.0f), dt * glm::pi<float>(), glm::vec3(0.0, 1.0, 0.0));
    dir = dir * rot;
}

}  // namespace actor
