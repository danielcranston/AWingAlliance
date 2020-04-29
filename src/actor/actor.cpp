#include <iostream>
#include <memory>
#include <bitset>
#include <map>

#include <actor/actor.h>
#include <GL/glew.h>

namespace actor
{
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
}  // namespace actor
