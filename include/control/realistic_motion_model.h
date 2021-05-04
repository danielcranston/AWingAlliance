#pragma once
#include <iostream>

#include <Eigen/Geometry>

#include "control/motion_model.h"

namespace control
{
class RealisticMotionModel
{
    // https://gafferongames.com/post/integration_basics/
    // https://gafferongames.com/post/physics_in_3d/
    // http://www.cs.cmu.edu/~baraff/pbm/rigid1.pdf
  public:
    RealisticMotionModel() = default;

    struct Derivative
    {
        Eigen::Vector3f v;
        Eigen::Quaternionf q;
        Eigen::Vector3f F;
        Eigen::Vector3f torque;
    };

    struct State
    {
        State()
          : x(Eigen::Vector3f::Zero()),
            q(Eigen::Quaternionf::Identity()),
            P(Eigen::Vector3f::Zero()),
            L(Eigen::Vector3f::Zero())
        {
        }

        // Main
        Eigen::Vector3f x;
        Eigen::Quaternionf q;
        Eigen::Vector3f P;  // Linear momentum
        Eigen::Vector3f L;  // Angular momentum

        // Constants
        float mass = 1.0f;
        Eigen::Matrix3f I_model = Eigen::Matrix3f::Identity();
        Eigen::Matrix3f I_model_inv = Eigen::Matrix3f::Identity();

        // Derived
        inline Eigen::Vector3f v()
        {
            return P / mass;
        }
        inline Eigen::Matrix3f R()
        {
            return q.matrix();
        }
        inline Eigen::Matrix3f I()
        {
            return R() * I_model * R().transpose();
        }
        inline Eigen::Matrix3f I_inv()
        {
            return R() * I_model_inv * R().transpose();
        }
        inline Eigen::Vector3f omega()
        {
            return I_model_inv * L;
        }

        Derivative dydt(Eigen::Vector3f F, Eigen::Vector3f torque)
        {
            // auto omega_cross = Eigen::AngleAxis<float>(omega().norm(), omega()).matrix();

            auto o = 0.5f * omega();
            auto o_q = Eigen::Quaternionf(0.0, o.x(), o.y(), o.z());

            return { v(), (o_q * q), F, torque };
        }
    };

    State state;

    Eigen::Isometry3f integrate(Eigen::Isometry3f pose, float t, float delta_time)
    {
        float ang_acc = t < 5.0f ? 1.0f : 0.0f;
        float lin_acc = t < 5.0f ? 10.0f : 0.0f;
        auto d = state.dydt({ 0.0, 0.0, -lin_acc }, { 0.0, ang_acc, 0.0 });

        std::cout << "state.L: " << state.L << std::endl;
        state.x += d.v * delta_time;
        // state.q = (state.R * d.R * delta_time).normalized();
        // state.q = (d.q * state.q).normalized();

        // Method1: Eigen does not support quaternion addition (not surprising)
        // state.q.x() += d.q.x() * delta_time;
        // state.q.y() += d.q.y() * delta_time;
        // state.q.z() += d.q.z() * delta_time;
        // state.q.w() += d.q.w() * delta_time;
        // state.q = state.q.normalized();

        // Method2: q_w = [ cos(|w|*dt/2) , sin(|w|*dt/2) * w/|w| ]
        auto o = state.omega();
        if (o.norm() > 10e-10)
        {
            float w = std::cos(o.norm() * delta_time / 2.0f);
            auto xyz = std::sin(o.norm() * delta_time / 2.0f) * o / o.norm();
            auto q_w = Eigen::Quaternionf(w, xyz.x(), xyz.y(), xyz.z());
            state.q = (state.q * q_w).normalized();
        }
        state.P += state.R() * d.F * delta_time;
        state.L += d.torque * delta_time;

        auto ret = Eigen::Isometry3f(state.q);
        ret.translation() = state.x;

        return ret;
    }

    Eigen::Isometry3f get_pose() const;

  private:
    Eigen::Isometry3f current_pose;

    Eigen::Vector3f current_lin_vel;
    Eigen::Vector3f current_lin_acc;
    Eigen::Vector3f current_ang_vel;  // = omega
    Eigen::Vector3f current_ang_acc;

    // State state;
};
}  // namespace control