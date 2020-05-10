#include "actor/camera.h"
#include "actor/ship.h"

Camera::Camera()
  : Actor({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, nullptr),
    fov_y(glm::radians(45.0f)),
    aspect_ratio(9.0f / 16.0f),
    near_distance(0.1f),
    far_distance(8192.0f)
{
}

void Camera::attach_to(actor::Actor* new_ptr) const
{
    actor_ptr = new_ptr;
}

void Camera::Update(const float dt)
{
    if (actor_ptr)
    {
        const glm::vec3& player_pos = actor_ptr->GetPosition();
        const glm::vec3& player_dir = actor_ptr->GetDirection();
        const glm::vec3& player_desired_dir =
            dynamic_cast<actor::Ship*>(actor_ptr)->GetDesiredDir();

        // Above
        // pos = player_pos + 40.0f * glm::vec3(0.0, 1.0, 0.01);
        // dir = player_pos;

        // Behind
        pos = player_pos + 5.0f * player_desired_dir - 25.0f * player_dir +
              5.0f * glm::vec3(0.0, 1.0, 0.0);
        dir = player_pos + 20.0f * player_dir;
        camMatrix = glm::lookAt(pos, dir, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else
    {
        camMatrix = glm::lookAt(pos, pos + dir, glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

void Camera::SetProjMatrix(const glm::mat4& new_projmat)
{
    projMatrix = new_projmat;
}

const glm::mat4& Camera::GetCamMatrix() const
{
    return camMatrix;
}

const glm::mat4& Camera::GetProjMatrix() const
{
    return projMatrix;
}
