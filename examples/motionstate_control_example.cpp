#include "rendering/context_manager.h"
#include "ecs/scene_factory.h"
#include "ecs/systems.h"
#include "input/key_event.h"

#include "ecs/components.h"

class MotionStateDataBuffer
{
  public:
    static constexpr int MAX_SAMPLES = 150;
    static constexpr int FLOATS_PER_SAMPLE = 9;
    int offset = 0;

    Eigen::Matrix<float, MAX_SAMPLES, FLOATS_PER_SAMPLE> data =
        Eigen::Matrix<float, MAX_SAMPLES, FLOATS_PER_SAMPLE>::Zero();

    float sample_rate = 1.0f / 30.0f;
    float last_sample_time = 0.0f;

    void register_stats(const geometry::MotionState& state, const float t)
    {
        if (t > last_sample_time + sample_rate)
        {
            auto R = state.orientation.toRotationMatrix();
            auto fwd = R.col(0).head<3>();
            auto left = R.col(1).head<3>();
            auto up = R.col(2).head<3>();

            data(offset, 0) = t;

            // Linear
            data(offset, 1) = fwd.dot(state.velocity);
            data(offset, 2) = fwd.dot(state.acceleration);

            // Roll
            data(offset, 3) = fwd.dot(state.angular_velocity);
            data(offset, 4) = fwd.dot(state.angular_acceleration);

            // Pitch
            data(offset, 5) = left.dot(state.angular_velocity);
            data(offset, 6) = left.dot(state.angular_acceleration);

            // Yaw
            data(offset, 7) = up.dot(state.angular_velocity);
            data(offset, 8) = up.dot(state.angular_acceleration);

            offset = (offset + 1) % MAX_SAMPLES;
            last_sample_time = t;
        }
    }

    inline std::vector<float> get_col(int col)
    {
        std::vector<float> out;
        out.reserve(MAX_SAMPLES);

        for (int i = offset; i < data.rows(); ++i)
        {
            out.push_back(data(i, col));
        }
        for (int i = 0; i < offset; ++i)
        {
            out.push_back(data(i, col));
        }

        return std::move(out);
    }
};

void make_plot(const std::string& title,
               const std::string& y_axis_unit,
               const float now,
               const float limit,
               const std::vector<float>& times,
               const std::vector<float>& velocity,
               const std::vector<float>& acceleration)
{
    if (ImPlot::BeginPlot(title.c_str(), ImVec2(-1, 180)))
    {
        std::string vel_label = "Velocity (" + y_axis_unit + "/s)";
        std::string acc_label = "Acceleration (" + y_axis_unit + "/s^2)";
        float history = 3.0f;

        // ImPlot::SetupAxes("time (s)", nullptr);
        ImPlot::SetupAxisLimits(ImAxis_X1, now - history, now, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -limit, limit, ImGuiCond_Always);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
        ImPlot::PlotLine(vel_label.c_str(), times.data(), velocity.data(), times.size());
        ImPlot::PlotLine(acc_label.c_str(), times.data(), acceleration.data(), times.size());
        ImPlot::EndPlot();
    }
}

void plot_motion_state_stats(const ecs::Scene& scene,
                             const rendering::ContextManager& context_manager,
                             MotionStateDataBuffer& buffer,
                             const float t)
{
    auto motion_state = scene.registry.get<MotionStateComponent>(scene.player_uid);
    auto fighter_component = scene.registry.get<FighterComponent>(scene.player_uid);

    buffer.register_stats(motion_state, t);

    std::vector<float> cont_times = buffer.get_col(0);

    context_manager.imgui_new_frame();

    float now = cont_times.size() ? cont_times.back() : t;

    ImGui::Begin("MotionState and Control Demo");
    ImGui::Text("Camera and ship controllers output appropriate");
    ImGui::Text("linear/angular accelerations (LQR for position,");
    ImGui::Text("PD-control for orientation) based on some");
    ImGui::Text("appropriate target state.");
    make_plot("Linear", "m", now, 60, cont_times, buffer.get_col(1), buffer.get_col(2));
    make_plot("Roll", "pi", now, 6.38, cont_times, buffer.get_col(3), buffer.get_col(4));
    make_plot("Pitch", "pi", now, 6.38, cont_times, buffer.get_col(5), buffer.get_col(6));
    make_plot("Yaw", "pi", now, 6.38, cont_times, buffer.get_col(7), buffer.get_col(8));
    // ImPlot::ShowDemoWindow();
    ImGui::End();

    context_manager.imgui_render();
}

int main(int argc, char* argv[])
{
    auto context_manager = rendering::ContextManager("Main Window", 1200, 900);

    auto scene = ecs::SceneFactory::create_from_scenario("scenario");

    MotionStateDataBuffer buffer;

    float current_time = SDL_GetTicks() / 1000.0f;
    const float dt = 1.0f / 60.f;
    float t = 0.0f;
    float accumulator = 0.0f;

    bool should_shutdown = false;
    while (!should_shutdown)
    {
        float new_time = SDL_GetTicks() / 1000.0f;
        float frameTime = new_time - current_time;
        current_time = new_time;

        accumulator += frameTime;

        while (accumulator >= dt)
        {
            ecs::systems::integrate(*scene, t, dt);
            accumulator -= dt;
            t += dt;
        }

        ecs::systems::render(*scene, t);

        plot_motion_state_stats(*scene, context_manager, buffer, t);

        SDL_GL_SwapWindow(context_manager.window);

        std::vector<KeyEvent> key_events;
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            context_manager.imgui_process_event(event);

            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
            {
                should_shutdown = true;
            }
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP && !event.key.repeat)
            {
                key_events.emplace_back(static_cast<char>(event.key.keysym.sym),
                                        event.type == SDL_KEYDOWN ? KeyEvent::Status::PRESSED :
                                                                    KeyEvent::Status::RELEASED);
            }
        }

        ecs::systems::handle_key_events(*scene, key_events);
    }
}
