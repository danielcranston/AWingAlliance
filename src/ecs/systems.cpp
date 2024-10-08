#include <GL/glew.h>

#include "rendering/draw.h"
#include "geometry/collision.h"
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

void render_visual(const rendering::ShaderProgram& shader_program,
                   const VisualComponent& visual_component,
                   const Eigen::Isometry3f& pose)
{
    shader_program.setUniformMatrix4fv("model_scale",
                                       visual_component.size ?
                                           geometry::to_scale_matrix(*visual_component.size) :
                                           Eigen::Matrix4f::Identity());

    const auto& meshes = visual_component.model->get_meshes();
    for (std::size_t i = 0; i < meshes.size(); ++i)
    {
        if (meshes[i].has_texture())
        {
            if (!visual_component.textures)
            {
                throw std::runtime_error("Mesh has texture, but no textures were provided");
            }

            rendering::draw_textured(shader_program,
                                     meshes[i],
                                     pose,
                                     *visual_component.textures.value()[i],  // FIXME
                                     GL_TRIANGLES);
        }
        else
        {
            rendering::draw_colored(shader_program,
                                    meshes[i],
                                    pose,
                                    visual_component.color ? *visual_component.color :
                                                             Eigen::Vector3f::Ones(),
                                    GL_TRIANGLES);
        }
    }
}
}  // namespace

namespace ecs::systems
{
void render(const Scene& scene, const float t)
{
    const auto& resource_manager = scene.resource_manager;
    const auto& camera = scene.registry.get<MotionStateComponent>(scene.camera_uid);
    const Eigen::Matrix4f camera_matrix = camera.pose().matrix().inverse();

    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    const auto& shader_skybox = *resource_manager.get_shader("skybox");
    shader_skybox.use();
    shader_skybox.setUniformMatrix4fv("camera", T_opengl_ros * camera_matrix);

    for (const auto [entity, skybox_component] : scene.registry.view<SkyboxComponent>().each())
    {
        std::ignore = entity;
        rendering::draw_textured(shader_skybox,
                                 skybox_component.model->get_meshes()[0],
                                 Eigen::Isometry3f::Identity(),
                                 *skybox_component.texture,
                                 GL_TRIANGLES);
    }

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    const auto& shader_model = *resource_manager.get_shader("model");

    shader_model.use();
    shader_model.setUniformMatrix4fv("camera", T_opengl_ros * camera_matrix);

    for (const auto [entity, motion_state, visual_component] :
         scene.registry.view<MotionStateComponent, VisualComponent>().each())
    {
        std::ignore = entity;
        render_visual(shader_model, visual_component, motion_state.pose());
    }

    const auto& shader_spark = *resource_manager.get_shader("spark");

    shader_spark.use();
    shader_model.setUniformMatrix4fv("camera", T_opengl_ros * camera_matrix);
    shader_spark.setUniform1f("time", t);
    glEnable(GL_BLEND);
    glDepthMask(false);

    const auto& quad_mesh = scene.resource_manager.get_model("quad")->get_meshes()[0];
    for (const auto [entity, motion_state, billboard_component] :
         scene.registry.view<MotionStateComponent, BillboardComponent>().each())
    {
        std::ignore = entity;
        shader_spark.setUniformMatrix4fv("model_scale", billboard_component.size);
        shader_spark.setUniform1f("start_time", billboard_component.birth_time);
        rendering::draw_colored(shader_spark,
                                quad_mesh,
                                motion_state.pose(),
                                Eigen::Vector3f(0.0f, 0.0f, 1.0f),
                                GL_TRIANGLES);
    }

    glDepthMask(true);
    glDisable(GL_BLEND);

    const auto& shader_spline = *resource_manager.get_shader("spline");
    shader_spline.use();
    shader_spline.setUniformMatrix4fv("camera", T_opengl_ros * camera_matrix);

    for (const auto [entity, spline_component] : scene.registry.view<SplineComponent>().each())
    {
        shader_spline.setUniformMatrix3x4fv("C", spline_component.curve.C);
        glDrawArrays(GL_POINTS, 0, 1);
    }
}

void integrate(Scene& scene, const float t, const float dt)
{
    // Update Camera: Invoke camera controller, (update linear/angular acceleration)
    if (scene.player_uid != entt::null)
    {
        auto fighter_motion_state = scene.registry.try_get<MotionStateComponent>(scene.player_uid);
        auto fighter_component = scene.registry.try_get<FighterComponent>(scene.player_uid);
        if (fighter_motion_state && fighter_component)
        {
            for (auto [entity, camera_component, camera_motion_state] :
                 scene.registry.view<CameraComponent, MotionStateComponent>().each())
            {
                std::ignore = camera_component;
                std::ignore = entity;
                camera_motion_state = scene.camera_controller.update(
                    camera_motion_state,
                    camera_component.get_target_state(*fighter_motion_state,
                                                      fighter_component->model->camera_poses[1]),
                    dt);

                audio::AudioContextManager::set_listener_pose(T_opengl_ros *
                                                              camera_motion_state.pose().matrix());
            }
        }
    }

    // Integrate all MotionStateComponents
    for (auto [entity, motion_state] : scene.registry.view<MotionStateComponent>().each())
    {
        std::ignore = entity;
        motion_state.integrate(dt);
    }

    // Update Fighters (invoke controller, react to controls) ...
    std::set<entt::entity> to_remove;
    for (auto [entity, fighter_component, motion_state] :
         scene.registry.view<FighterComponent, MotionStateComponent>().each())
    {
        if (fighter_component.alive())
        {
            if (const auto dispatches = fighter_component.try_fire_laser(t))
            {
                for (const auto& dispatch : *dispatches)
                {
                    auto laser_pose = motion_state.pose() * dispatch.first;
                    scene.register_laser(Eigen::Vector3f(laser_pose.translation()),
                                         Eigen::Quaternionf(laser_pose.linear()),
                                         fighter_component.model,
                                         dispatch.second.size,
                                         dispatch.second.color,
                                         dispatch.second.speed,
                                         entity);

                    if (!fighter_component.model->sounds.laser.empty())
                    {
                        fighter_component.fire_sound_source->play(*scene.resource_manager.get_sound(
                            fighter_component.model->sounds.laser));
                    }
                }
            }

            if (!fighter_component.model->sounds.engine.empty() &&
                !fighter_component.engine_sound_source->is_playing())
            {
                fighter_component.engine_sound_source->play(
                    *scene.resource_manager.get_sound(fighter_component.model->sounds.engine));
            }

            fighter_component.fire_sound_source->set_pose(T_opengl_ros *
                                                          motion_state.pose().matrix());
            fighter_component.engine_sound_source->set_pose(T_opengl_ros *
                                                            motion_state.pose().matrix());

            fighter_component.try_toggle_fire_mode();

            motion_state = scene.ship_controller.update(
                motion_state, fighter_component.get_target_state(motion_state), dt);
            fighter_component.model->apply_motion_limits(motion_state);
        }
        else
        {
            if (fighter_component.time_of_death.value() + 2.0f < t)
            {
                std::vector<Eigen::AngleAxisf> offsets = {
                    Eigen::AngleAxisf(M_PI / 2.0f, Eigen::Vector3f::UnitX()),
                    Eigen::AngleAxisf(M_PI / 2.0f, Eigen::Vector3f::UnitY()),
                    Eigen::AngleAxisf(M_PI / 2.0f, Eigen::Vector3f::UnitZ())
                };

                for (const auto& offset : offsets)
                {
                    scene.register_billboard(motion_state.position,
                                             offset * motion_state.orientation,
                                             { 0.0f, 60.0f, 60.0f },
                                             2.0f,
                                             t);
                }
                to_remove.insert(entity);
            }
        }
    }
    // ... and remove destroyed ships
    scene.registry.destroy(to_remove.begin(), to_remove.end());

    // Tick state machines
    // for (auto [entity, motion_state, state_machine_component] :
    //      scene.registry.view<MotionStateComponent, RoamingStateMachineComponent>().each())
    // {
    //     std::ignore = entity;

    //     // Poll state, create new roaming goal if necessary.
    //     if (state_machine_component.instance.isActive)
    // }

    // Calculate, detect and react to collisions ...
    to_remove.clear();
    auto fighter_view =
        scene.registry.view<FighterComponent, MotionStateComponent, HealthComponent>().each();
    auto laser_view = scene.registry.view<LaserComponent, MotionStateComponent>().each();
    for (auto [laser_entity, laser_component, laser_motion] : laser_view)
    {
        for (auto [fighter_entity, fighter_component, fighter_motion, health_component] :
             fighter_view)
        {
            if (laser_component.producer != fighter_entity)
            {
                auto laser_speed = laser_motion.velocity.dot(laser_motion.fwd());

                if (geometry::ray_aabb_test(laser_motion.pose(),
                                            laser_component.length / 2.0f,
                                            -laser_component.length / 2.0f - laser_speed * dt,
                                            fighter_motion.pose(),
                                            fighter_component.model->dimensions))
                {
                    health_component.take_damage(laser_component.fighter_model->laser_info.damage);
                    std::cout << "laser hit " << fighter_component.name
                              << ". shields: " << health_component.shields
                              << ", hull: " << health_component.hull << std::endl;
                    if (health_component.hull <= 0 && fighter_component.alive())
                    {
                        fighter_component.time_of_death = t;
                        fighter_motion.angular_velocity =
                            fighter_motion.orientation * Eigen::Vector3f(M_PI_2, 0.0f, 0.0f);
                    }

                    auto backwards_offset = laser_motion.orientation *
                                            Eigen::Vector3f(-laser_speed * dt, 0.0f, 0.0f) / 2.0f;
                    auto& impact_info = laser_component.fighter_model->laser_info.impact_info;

                    scene.register_billboard(laser_motion.position + backwards_offset,
                                             laser_motion.orientation,
                                             impact_info.size,
                                             impact_info.duration,
                                             t);

                    if (!fighter_component.model->sounds.hit.empty())
                    {
                        scene.register_sound_effect(fighter_component.model->sounds.hit,
                                                    fighter_motion.position,
                                                    fighter_motion.orientation);
                    }

                    to_remove.insert(laser_entity);
                    break;
                }
            }
        }
    }
    // ... and remove lasers that hit something
    scene.registry.destroy(to_remove.begin(), to_remove.end());

    // Check if any SoundEffects have finished playing ...
    to_remove.clear();
    for (auto [entity, sound_effect_component] : scene.registry.view<SoundEffectComponent>().each())
    {
        if (!sound_effect_component.sound_source->is_playing())
        {
            to_remove.insert(entity);
        }
    }
    // ... and remove those who have
    scene.registry.destroy(to_remove.begin(), to_remove.end());

    // Update Billboards ...
    to_remove.clear();
    for (auto [entity, billboard_component] : scene.registry.view<BillboardComponent>().each())
    {
        if (billboard_component.birth_time + billboard_component.duration < t)
        {
            to_remove.insert(entity);
        }
    }
    // ... and remove those who have expired
    scene.registry.destroy(to_remove.begin(), to_remove.end());

    audio::AudioContextManager::update();
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
