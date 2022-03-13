#include <GL/glew.h>

#include "rendering/draw.h"
#include "ecs/components.h"
#include "ecs/systems.h"

namespace
{
// Converts data from a frame expressed in ROS convention (X fwd, Y left)
// (https://www.ros.org/reps/rep-0103.html#coordinate-frame-conventions)
// into an equivalent frame in OpenGL convention (-Z fwd, -X left)
// ()
static const Eigen::Matrix4f T_opengl_ros = []() {
    auto out = Eigen::Matrix4f();
    out.row(0) << 0, -1, 0, 0;
    out.row(1) << 0, 0, 1, 0;
    out.row(2) << -1, 0, 0, 0;
    out.row(3) << 0, 0, 0, 1;
    return out;
}();
}  // namespace

namespace ecs::systems
{
void render(const Scene& scene)
{
    const auto& resource_manager = scene.resource_manager;
    const auto& camera = scene.registry.get<MotionStateComponent>(scene.camera_uid);
    const Eigen::Matrix4f camera_matrix = camera.pose().matrix().inverse();

    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    const auto& shader_skybox = resource_manager.get_shader("skybox").get();
    shader_skybox.use();
    shader_skybox.setUniformMatrix4fv("camera", T_opengl_ros * camera_matrix);

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
    shader_model.setUniformMatrix4fv("camera", T_opengl_ros * camera_matrix);

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

void integrate(Scene& scene, const float t, const float dt)
{
    auto target_pose = Eigen::Isometry3f(Eigen::Quaternionf(0.966, 0, 0.259, 0));
    target_pose.translation() = Eigen::Vector3f(15.0f, 0.0f, 1.68f);

    for (auto [entity, camera_component, motion_state] :
         scene.registry.view<CameraComponent, MotionStateComponent>().each())
    {
        motion_state = scene.camera_controller.update(motion_state, target_pose, 0, dt);
    }

    for (auto [entity, motion_state] : scene.registry.view<MotionStateComponent>().each())
    {
        motion_state.integrate(dt);
    }

    for (auto [entity, fighter_component] : scene.registry.view<FighterComponent>().each())
    {
        if (fighter_component.firing())
        {
            if (auto dispatches = fighter_component.try_fire_laser(t))
            {
                std::cout << "fired " << dispatches->size() << " laser(s)" << std::endl;
            }
        }
    }
}

void handle_key_events(Scene& scene, const std::vector<KeyEvent>& key_events)
{
    if (scene.player_uid != entt::null)
    {
        if (FighterComponent* fighter_component =
                scene.registry.try_get<FighterComponent>(scene.player_uid))
        {
            for (const auto& event : key_events)
            {
                fighter_component->input.handle_key_event(event);
            }
        }
    }
}

}  // namespace ecs::systems
