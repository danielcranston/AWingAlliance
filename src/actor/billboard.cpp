#include "actor/billboard.h"

namespace actor
{
Billboard::Billboard(const glm::vec3 pos,
                     const glm::vec3 dir,
                     const Type& type,
                     const unsigned int tex_id,
                     const float current_time)
  : Actor(pos, dir, nullptr), type(type), tex_id(tex_id), time_of_birth(current_time)
{
}

const float Billboard::GetExpireTime() const
{
    return time_of_birth + life_duration;
}
const float Billboard::GetTimeOfBirth() const
{
    return time_of_birth;
}

const glm::vec2& Billboard::GetScale() const
{
    return scale;
}

void Billboard::Update(const float dt)
{
    const glm::vec3 cam_pos = GetCameraPosFunc();
    SetDirection(glm::normalize(cam_pos - pos));
}

void Billboard::RegisterBillboard(std::list<Billboard>& billboard_list,
                                  const Billboard& new_billboard)
{
    billboard_list.push_back(new_billboard);
}

// GetCameraPosFunc will be assigned by the GameState, but first needs to be defined here else we
// get a linker error.
std::function<const glm::vec3&()> Billboard::GetCameraPosFunc = []() -> const glm::vec3& {
    throw std::runtime_error("You need to set set Billboard::GetCameraPosFunc by yourself");
};
;
}  // namespace actor
