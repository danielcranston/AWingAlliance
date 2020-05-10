#include "actor/billboard.h"

namespace actor
{
std::unique_ptr<Billboard> Billboard::Create(const glm::vec3 pos,
                                             const glm::vec3 dir,
                                             const Type& type,
                                             const unsigned int tex_id)
{
    return std::unique_ptr<Billboard>(new Billboard(pos, dir, type, tex_id));
}

Billboard::Billboard(const glm::vec3 pos,
                     const glm::vec3 dir,
                     const Type& type,
                     const unsigned int tex_id)
  : Actor(pos, dir, nullptr),
    type(type),
    tex_id(tex_id),
    time_of_birth(std::chrono::system_clock::now())
{
}

const std::chrono::system_clock::time_point Billboard::GetExpireTime() const
{
    return time_of_birth + life_duration;
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
std::function<const glm::vec3&()> Billboard::GetCameraPosFunc = nullptr;
}  // namespace actor
