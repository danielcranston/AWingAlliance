#include "control/ship_controller.h"

#include <iostream>
#include "geometry/geometry.h"

namespace control
{
ShipController::ShipController()
{
}

geometry::MotionState ShipController::update(const geometry::MotionState& state,
                                             const geometry::MotionState& target_state,
                                             const float dt)
{
    auto out = orientation_controller.update(state, target_state, dt);
    out = velocity_controller.update(out, target_state, dt);

    return out;
}

}  // namespace control
