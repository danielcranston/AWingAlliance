#include "control/camera_controller.h"

namespace control
{
CameraController::CameraController()
{
}

geometry::MotionState CameraController::update(const geometry::MotionState& state,
                                               const geometry::MotionState& target_state,
                                               const float dt)
{
    auto out = orientation_controller.update(state, target_state, dt);
    out = position_controller.update(out, target_state, dt);

    return out;
}
}  // namespace control
