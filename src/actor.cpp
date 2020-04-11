#include <iostream>
#include <memory>
#include <bitset>
#include <map>

#include <actor.h>
#include <GL/glew.h>

//  #define GLM_ENABLE_EXPERIMENTAL
//  #include <glm/gtx/string_cast.hpp>

extern GLuint program;

namespace actor
{
Actor::Actor(const glm::vec3& p, const glm::vec3& d, const Model* mdl)
  : pos{ p }, dir{ glm::normalize(d) }, model{ mdl }
{
}

void Actor::Draw(glm::mat4 camprojMat)
{
    // Construct Model Matrix from Position and Viewing Direction
    mdlMatrix = glm::inverse(glm::lookAt(glm::vec3(0.0, 0.0, 0.0), -dir, glm::vec3(0.0, 1.0, 0.0)));
    mdlMatrix = glm::translate(glm::mat4(1.0F), pos) * mdlMatrix;

    glm::mat4 mvp = camprojMat * mdlMatrix;
    model->Draw(mvp, program, glm::vec3(0.0, 0.0, 0.0));
}

void Actor::SetPosition(glm::vec3 p)
{
    pos = p;
}

}  // namespace actor