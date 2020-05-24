#pragma once

#include "actor/actor.h"

#include <chrono>
#include <list>
#include <functional>

namespace actor
{
class Billboard : public Actor
{
  public:
    enum class Type
    {
        DIRECTIONAL,
        CAMERA_FACING
    };
    void Update(const float dt) override;

    static std::function<const glm::vec3&()> GetCameraPosFunc;
    static void RegisterBillboard(std::list<Billboard>& billboard_list,
                                  const Billboard& new_billboard);

    const float GetExpireTime() const;
    const float GetTimeOfBirth() const;
    const glm::vec2& GetScale() const;

    explicit Billboard(const glm::vec3 pos,
                       const glm::vec3 dir,
                       const Type& type,
                       const unsigned int tex_id,
                       const float current_time);

  private:
    const Type type;
    unsigned int tex_id;
    glm::vec2 scale = { 6.0f, 6.0f };

    float time_of_birth;
    float life_duration = 1.0f;
};
}  // namespace actor
