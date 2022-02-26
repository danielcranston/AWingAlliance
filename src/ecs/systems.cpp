#include <GL/glew.h>

#include "rendering/draw.h"
#include "ecs/components.h"
#include "ecs/systems.h"

namespace ecs::systems
{
void render(const Scene& scene)
{
    const auto& resource_manager = scene.resource_manager;
    const auto& camera = scene.registry.get<MotionStateComponent>(scene.current_camera_uid);
    const Eigen::Matrix4f camera_matrix = camera.pose().matrix().inverse();

    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    const auto& shader_skybox = resource_manager.get_shader("skybox").get();
    shader_skybox.use();
    shader_skybox.setUniformMatrix4fv("camera", camera_matrix);

    for (const auto [entity, skybox_component] : scene.registry.view<SkyboxComponent>().each())
    {
        rendering::draw_textured(shader_skybox,
                                 skybox_component.model.get().get_meshes()[0],
                                 Eigen::Isometry3f::Identity(),
                                 skybox_component.texture.get(),
                                 GL_TRIANGLES);
    }

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    const auto& shader_model = resource_manager.get_shader("model").get();

    shader_model.use();
    shader_model.setUniformMatrix4fv("camera", camera_matrix);

    auto view = scene.registry.view<MotionStateComponent, VisualComponent>();
    for (const auto [entity, motion_state, visual_component] : view.each())
    {
        const auto& meshes = visual_component.model->get_meshes();
        for (int i = 0; i < meshes.size(); ++i)
        {
            if (meshes[i].has_texture())
            {
                rendering::draw_textured(shader_model,
                                         meshes[i],
                                         motion_state.pose(),
                                         visual_component.textures[i].get(),
                                         GL_TRIANGLES);
            }
            else
            {
                rendering::draw_colored(shader_model,
                                        meshes[i],
                                        motion_state.pose(),
                                        { 0.7f, 0.0f, 0.0f },
                                        GL_TRIANGLES);
            }
        }
    }
}

void integrate(Scene& scene, const float dt)
{
    auto target_pose = Eigen::Isometry3f(Eigen::Quaternionf(0.966, 0, 0.259, 0));
    target_pose.translation() = Eigen::Vector3f(0.0f, -1.68f, -15.0f);

    for (auto [entity, camera_component, motion_state] :
         scene.registry.view<CameraComponent, MotionStateComponent>().each())
    {
        motion_state = scene.camera_controller.update(motion_state, target_pose, 0, dt);
    }

    auto view = scene.registry.view<MotionStateComponent>();
    for (auto [entity, motion_state] : view.each())
    {
        motion_state.integrate(dt);
    }
}

}  // namespace ecs::systems