#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace control
{
Eigen::MatrixXf solveRiccatiArimotoPotter(const Eigen::MatrixXf& A,
                                          const Eigen::MatrixXf& B,
                                          const Eigen::MatrixXf& Q,
                                          const Eigen::MatrixXf& R);

Eigen::MatrixXf LQR(const Eigen::MatrixXf& A,
                    const Eigen::MatrixXf& B,
                    const Eigen::MatrixXf& Q,
                    const Eigen::MatrixXf& R);

template <typename Scalar, int STATE_DIM, int CONTROL_DIM>
class LQRController
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    LQRController(const Eigen::Matrix<Scalar, STATE_DIM, STATE_DIM>& A,
                  const Eigen::Matrix<Scalar, STATE_DIM, CONTROL_DIM>& B,
                  const Eigen::Matrix<Scalar, STATE_DIM, STATE_DIM>& Q,
                  const Eigen::Matrix<Scalar, CONTROL_DIM, CONTROL_DIM>& R)
      : A(A),
        B(B),
        Q(Q),
        R(R),
        K(LQR(A, B, Q, R)),
        state_dim(STATE_DIM),
        control_dim(CONTROL_DIM){};

    ~LQRController() = default;

    Eigen::Matrix<Scalar, CONTROL_DIM, 1>
        compute_control(Eigen::Matrix<Scalar, STATE_DIM, 1>& x,
                        Eigen::Matrix<Scalar, STATE_DIM, 1>& x_goal)
    {
        return -K * (x - x_goal);
    }

    Eigen::Matrix<Scalar, STATE_DIM, 1>
        compute_state_derivative(Eigen::Ref<Eigen::Matrix<Scalar, CONTROL_DIM, 1>> u) const
    {
        return A * x + B * u;
    }

    void update(const Scalar dt)
    {
        auto u = compute_control(x, x_goal);
        auto xdot = compute_state_derivative(u);
        x = x + dt * xdot;
    }

    void set_state(Eigen::Ref<const Eigen::Matrix<Scalar, STATE_DIM, 1>> new_x)
    {
        x = new_x;
    }

    Eigen::Matrix<Scalar, STATE_DIM, 1> get_state() const
    {
        return x;
    }

    void set_goal_state(Eigen::Ref<const Eigen::Matrix<Scalar, STATE_DIM, 1>> new_x)
    {
        x_goal = new_x;
    }

    Eigen::Matrix<Scalar, STATE_DIM, 1> get_goal_state() const
    {
        return x_goal;
    }

  private:
    Eigen::Matrix<Scalar, STATE_DIM, 1> x;
    Eigen::Matrix<Scalar, STATE_DIM, 1> x_goal;
    Eigen::Matrix<Scalar, STATE_DIM, STATE_DIM> A;
    Eigen::Matrix<Scalar, STATE_DIM, CONTROL_DIM> B;
    Eigen::Matrix<Scalar, STATE_DIM, STATE_DIM> Q;
    Eigen::Matrix<Scalar, CONTROL_DIM, CONTROL_DIM> R;
    Eigen::Matrix<Scalar, CONTROL_DIM, STATE_DIM> K;
    int state_dim;
    int control_dim;
};

}  // namespace control