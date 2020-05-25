#include "actor/camera.h"

Camera::Camera()
  : Actor({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, nullptr),
    fov_y(glm::radians(45.0f)),
    aspect_ratio(9.0f / 16.0f),
    near_distance(0.1f),
    far_distance(8192.0f)
{
}

void Camera::Update(const float dt)
{
    const auto ret = placement_func();
    pos = ret.first;
    dir = ret.second;
    camMatrix = glm::lookAt(pos, dir, glm::vec3(0.0f, 1.0f, 0.0f));
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

// Need a definition to avoid linker errors, just declaration isn't enough.
std::function<std::pair<glm::vec3, glm::vec3>()> Camera::placement_func =
    []() -> std::pair<glm::vec3, glm::vec3> {
    throw std::runtime_error("You need to set define your own function to position the camera.");
};
