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
class System
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    System() : x(), A(), B(), state_dim(STATE_DIM), control_dim(CONTROL_DIM){};
    System(Eigen::Ref<Eigen::Matrix<Scalar, STATE_DIM, STATE_DIM>> A,
           Eigen::Ref<Eigen::Matrix<Scalar, STATE_DIM, CONTROL_DIM>> B)
      : x(), A(A), B(B), state_dim(STATE_DIM), control_dim(CONTROL_DIM)
    {
    }
    ~System() = default;
    Eigen::Matrix<Scalar, STATE_DIM, 1>
        compute_state_derivative(Eigen::Ref<Eigen::Matrix<Scalar, CONTROL_DIM, 1>> u) const
    {
        return A * x + B * u;
    }

    Eigen::Ref<Eigen::Matrix<Scalar, STATE_DIM, 1>>
        integrate(Eigen::Ref<Eigen::Matrix<Scalar, CONTROL_DIM, 1>> u, Scalar step)
    {
        auto xdot = compute_state_derivative(u);
        x = x + step * xdot;
        return x;
    }

    void set_state_vector(Eigen::Ref<Eigen::Matrix<Scalar, STATE_DIM, 1>> new_x)
    {
        x = new_x;
    }

    Eigen::Matrix<Scalar, STATE_DIM, 1> get_state() const
    {
        return x;
    }

    Eigen::Matrix<Scalar, STATE_DIM, STATE_DIM> get_A() const
    {
        return A;
    }

    Eigen::Matrix<Scalar, STATE_DIM, CONTROL_DIM> get_B() const
    {
        return B;
    }

    int get_state_dim() const
    {
        return state_dim;
    }

    int get_control_dim() const
    {
        return control_dim;
    }

  private:
    Eigen::Matrix<Scalar, STATE_DIM, 1> x;
    Eigen::Matrix<Scalar, STATE_DIM, STATE_DIM> A;
    Eigen::Matrix<Scalar, STATE_DIM, CONTROL_DIM> B;
    int state_dim;
    int control_dim;
};

template <typename Scalar, int STATE_DIM, int CONTROL_DIM>
class LQRController
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    LQRController(const Eigen::Matrix<Scalar, STATE_DIM, STATE_DIM>& A,
                  const Eigen::Matrix<Scalar, STATE_DIM, CONTROL_DIM>& B,
                  const Eigen::Matrix<Scalar, STATE_DIM, STATE_DIM>& Q,
                  const Eigen::Matrix<Scalar, CONTROL_DIM, CONTROL_DIM>& R)
      : Q(Q), R(R), K(LQR(A, B, Q, R)){};

    ~LQRController() = default;

    Eigen::Matrix<Scalar, CONTROL_DIM, 1>
        compute_control(Eigen::Matrix<Scalar, STATE_DIM, 1>& x,
                        Eigen::Matrix<Scalar, STATE_DIM, 1>& x_goal)
    {
        return -K * (x - x_goal);
    }

    Eigen::Matrix<Scalar, STATE_DIM, STATE_DIM> get_Q()
    {
        return Q;
    }

    Eigen::Matrix<Scalar, CONTROL_DIM, CONTROL_DIM> get_R()
    {
        return R;
    }

    Eigen::Matrix<Scalar, CONTROL_DIM, STATE_DIM> get_K()
    {
        return K;
    }

  private:
    Eigen::Matrix<Scalar, STATE_DIM, STATE_DIM> Q;
    Eigen::Matrix<Scalar, CONTROL_DIM, CONTROL_DIM> R;
    Eigen::Matrix<Scalar, CONTROL_DIM, STATE_DIM> K;
};

}  // namespace control