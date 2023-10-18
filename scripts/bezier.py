import numpy as np
import matplotlib.pyplot as plt


class CubicBezierCurve:
    def __init__(self, c1, c2, c3, c4):
        self.M = np.array(
            [
                [-1, 3, -3, 1],
                [3, -6, 3, 0],
                [-3, 3, 0, 0],
                [1, 0, 0, 0],
            ]
        )

        self.C = np.vstack((c1, c2, c3, c4))

    def __call__(self, t):
        T = np.array([t**3, t**2, t, 1])

        return T @ self.M @ self.C


B = CubicBezierCurve([0, 0], [0, 0.5], [0.5, 0.5], [0.5, 1.0])

points = np.array([B(t) for t in np.linspace(0, 1, 101, endpoint=True)])
plt.plot(points[:, 0], points[:, 1])
plt.show()
