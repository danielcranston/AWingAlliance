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
}  // namespace control
