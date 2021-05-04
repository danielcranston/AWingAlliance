#pragma one

#include <Eigen/Geometry>

// https://pomax.github.io/bezierinfo/

/*

https://pomax.github.io/bezierinfo/legendre-gauss.html#n10

1 	0.2955242247147529 	-0.1488743389816312
2 	0.2955242247147529 	0.1488743389816312
3 	0.2692667193099963 	-0.4333953941292472
4 	0.2692667193099963 	0.4333953941292472
5 	0.2190863625159820 	-0.6794095682990244
6 	0.2190863625159820 	0.6794095682990244
7 	0.1494513491505806 	-0.8650633666889845
8 	0.1494513491505806 	0.8650633666889845
9 	0.0666713443086881 	-0.9739065285171717
10 	0.0666713443086881 	0.9739065285171717

*/

class BezierCurve
{
  public:
    BezierCurve(Eigen::Vector3f p0, Eigen::Vector3f p1, Eigen::Vector3f p2, Eigen::Vector3f p3);

    Eigen::Vector3f operator()(float u);

  private:
    Eigen::Vector3f p0;
    Eigen::Vector3f p1;
    Eigen::Vector3f p2;
    Eigen::Vector3f p3;
};