#pragma once

#include <hfsm2/machine.hpp>

#include "geometry/spline.h"

namespace sm
{
struct RoamingStateMachineContext
{
    std::optional<geometry::CubicBezierCurve> spline;
};

using M = hfsm2::MachineT<hfsm2::Config::ContextT<RoamingStateMachineContext>>;

struct WaitForNewTrajectory;
struct FollowTrajectoryState;

using RoamingStateMachine = M::PeerRoot<WaitForNewTrajectory, FollowTrajectoryState>;

struct NewTrajectoryEvent
{
    NewTrajectoryEvent(geometry::CubicBezierCurve spline) : spline(std::move(spline)){};

    geometry::CubicBezierCurve spline;
};

struct WaitForNewTrajectory : RoamingStateMachine::State
{
    void enter(Control&) noexcept
    {
        // std::cout << "  WaitForNewTrajectory" << std::endl;
    }
    void update(FullControl&)
    {
        // std::cout << "  WaitForNewTrajectory waiting for new trajectory ..." << std::endl;
    }

    void react(const NewTrajectoryEvent& event, FullControl& control)
    {
        // std::cout << "  WaitForNewTrajectory got new trajectory !" << std::endl;
        control.context().spline = event.spline;
        control.changeTo<FollowTrajectoryState>();
    }
};

struct FollowTrajectoryState : RoamingStateMachine::State
{
    void enter(Control&) noexcept
    {
        // std::cout << "  FollowTrajectoryState" << std::endl;
    }

    void update(FullControl& control)
    {
        control.changeTo<WaitForNewTrajectory>();
    }
};

}  // namespace sm
