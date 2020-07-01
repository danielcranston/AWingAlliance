#include "geometry.h"

namespace geometry
{
glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
{
    using namespace glm;
    start = normalize(start);
    dest = normalize(dest);

    float cosTheta = dot(start, dest);
    vec3 rotationAxis;

    if (cosTheta < -1 + 0.001f)
    {
        // special case when vectors in opposite directions
        return quat(0.0f, 0.0f, 0.0f, 1.0f);
    }

    rotationAxis = cross(start, dest);

    float s = sqrt((1 + cosTheta) * 2);
    float invs = 1 / s;

    return quat(s * 0.5f, rotationAxis.x * invs, rotationAxis.y * invs, rotationAxis.z * invs);
}
}  // namespace geometry
