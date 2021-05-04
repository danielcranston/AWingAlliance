#include "control/lqr.h"

namespace control
{
// https://github.com/TakaHoribe/Riccati_Solver
Eigen::MatrixXf solveRiccatiArimotoPotter(const Eigen::MatrixXf& A,
                                          const Eigen::MatrixXf& B,
                                          const Eigen::MatrixXf& Q,
                                          const Eigen::MatrixXf& R)
{
    const uint dim_x = A.rows();

    // set Hamilton matrix
    Eigen::MatrixXf Ham = Eigen::MatrixXf::Zero(2 * dim_x, 2 * dim_x);
    Ham << A, -B * R.inverse() * B.transpose(), -Q, -A.transpose();

    // Extract eigenvectors
    Eigen::EigenSolver<Eigen::MatrixXf> Eigs(Ham);
    Eigen::MatrixXcf eigvec = Eigen::MatrixXcf::Zero(2 * dim_x, dim_x);
    int j = 0;
    for (int i = 0; i < 2 * dim_x; ++i)
    {
        if (Eigs.eigenvalues()[i].real() < 0.)
        {
            eigvec.col(j) = Eigs.eigenvectors().block(0, i, 2 * dim_x, 1);
            ++j;
        }
    }

    // calculate S with stable eigen vector matrix
    Eigen::MatrixXcf Vs_1, Vs_2;
    Vs_1 = eigvec.block(0, 0, dim_x, dim_x);
    Vs_2 = eigvec.block(dim_x, 0, dim_x, dim_x);
    auto S = (Vs_2 * Vs_1.inverse()).real();

    return S;
}

Eigen::MatrixXf LQR(const Eigen::MatrixXf& A,
                    const Eigen::MatrixXf& B,
                    const Eigen::MatrixXf& Q,
                    const Eigen::MatrixXf& R)
{
    Eigen::MatrixXf S = solveRiccatiArimotoPotter(A, B, Q, R);
    if (S.hasNaN())
    {
        throw std::runtime_error("Unable to solve CARE for LQR controller");
    }

    auto K = R.inverse() * B.transpose() * S;

    return K;
}

}  // namespace control