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

    static std::unique_ptr<Billboard>
    Create(const glm::vec3 pos, const glm::vec3 dir, const Type& type, const unsigned int tex_id);

    void Update(const float dt) override;

    static std::function<const glm::vec3&()> GetCameraPosFunc;
    static void RegisterBillboard(std::list<Billboard>& billboard_list,
                                  const Billboard& new_billboard);

    const std::chrono::system_clock::time_point GetExpireTime() const;
    const glm::vec2& GetScale() const;

    explicit Billboard(const glm::vec3 pos,
                       const glm::vec3 dir,
                       const Type& type,
                       const unsigned int tex_id);

  private:
    const Type type;
    unsigned int tex_id;
    glm::vec2 scale = { 3.0f, 3.0f };

    std::chrono::system_clock::time_point time_of_birth;
    std::chrono::system_clock::duration life_duration = std::chrono::milliseconds(500);
};
}  // namespace actor
