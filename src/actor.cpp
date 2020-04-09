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
Actor::Actor()
{
}

Actor::Actor(const glm::vec3& p,
             const glm::vec3& d,
             const std::vector<std::pair<Model*, glm::mat4>>& part_vector)
  : pos{ p }, dir{ glm::normalize(d) }
{
    for (auto const& p : part_vector)
    {
        parts.push_back({ p.first, p.second });
    }
}

void Actor::Draw(glm::mat4 camprojMat)
{
    // Construct Model Matrix from Position and Viewing Direction
    mdlMatrix = glm::inverse(glm::lookAt(glm::vec3(0.0, 0.0, 0.0), -dir, glm::vec3(0.0, 1.0, 0.0)));
    mdlMatrix = glm::translate(glm::mat4(1.0F), pos) * mdlMatrix;

    // Draw each part
    glm::mat4 mvp = camprojMat * mdlMatrix;
    GLsizei stride = (3 + 3 + 3 + 2) * sizeof(float);
    for (Part p : parts)
    {
        glm::mat4 mvp2 = mvp * p.pose;
        p.model->Draw(mvp2, program, glm::vec3(0.0, 0.0, 0.0));
    }
}

void Actor::SetPosition(glm::vec3 p)
{
    pos = p;
}

}  // namespace actor