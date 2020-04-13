#include <iostream>
#include <memory>
#include <bitset>
#include <map>

#include <actor.h>
#include <GL/glew.h>

//  #define GLM_ENABLE_EXPERIMENTAL
//  #include <glm/gtx/string_cast.hpp>

// extern GLuint program;

namespace actor
{
Actor::Actor(const glm::vec3& p, const glm::vec3& d, const Model* mdl)
  : pos{ p }, dir{ glm::normalize(d) }, model{ mdl }
{
}

std::unique_ptr<Actor> Actor::Create(const glm::vec3& p, const glm::vec3& d, const Model* mdl)
{
    return std::make_unique<Actor>(p, d, mdl);
}

const Model* Actor::GetModel() const
{
    return model;
}

void Actor::SetPosition(glm::vec3 p)
{
    pos = p;
}

}  // namespace actor
