#pragma once

#include <actor.h>
#include <spline.h>

namespace actor
{
class Fighter : public Actor
{
  public:
    static std::unique_ptr<Fighter> Create(const glm::vec3& p,
                                           const glm::vec3& d,
                                           const Model* mdl);
    Fighter(const glm::vec3& p, const glm::vec3& d, const Model* mdl);

    void Update(const float speedChange, const float turnChange, const float dt);
    void Update(const std::bitset<8>& keyboardInfo, float dt);
    void Update_Roaming(float t);

    bool bDrawBBox;
    bool bDrawSpline;

  private:
    Spline s;
    glm::vec3 color = { 0.7, 0.0, 0.0 };

    float currentSpeed;
    float currentTurnSpeed;
    const float maxSpeed;
    const float maxTurnSpeed;
};

}  // namespace actor