#include <spline.h>

glm::vec3 randomVec3(float xMax, float yMax, float zMax)
{
    float x = 2 * xMax * (std::rand() / (1.0 * RAND_MAX)) - xMax;
    float y = 2 * yMax * (std::rand() / (1.0 * RAND_MAX)) - yMax;
    float z = 2 * zMax * (std::rand() / (1.0 * RAND_MAX)) - zMax;
    return glm::vec3(x, y, z);
}

Spline::Waypoint::Waypoint(const glm::vec3& pos, const glm::vec3 dir, const float speed)
  : pos(pos), dir(dir), speed(speed)
{
}

Spline::Spline(const Waypoint start, const Waypoint end) : start(start), end(end)
{
    Update();
}

void Spline::Update()
{
    const glm::vec3 p0 = start.pos - (start.dir * start.speed);
    const glm::vec3 p1 = start.pos;
    const glm::vec3 p2 = end.pos;
    const glm::vec3 p3 = end.pos + (end.dir * end.speed);

    const glm::mat4 P = { p0.x, p0.y, p0.z, 1.0, p1.x, p1.y, p1.z, 1.0,
                          p2.x, p2.y, p2.z, 1.0, p3.x, p3.y, p3.z, 1.0 };

    const glm::mat4 M = { -0.5, 3 / 2.0, -3 / 2.0, 0.5, 1.0, -5 / 2.0, 2,   -0.5,
                          -0.5, 0.0,     0.5,      0,   0.0, 1.0,      0.0, 0.0 };

    const glm::mat4 Mp = { 0.0, 0.0,  0.0, 0.0,  -3 / 2.0, 9 / 2.0, -9 / 2.0, 3 / 2.0,
                           2.0, -5.0, 4.0, -1.0, -0.5,     0.0,     0.5,      0.0 };

    MP = P * M;
    MpP = P * Mp;
}

std::pair<glm::vec3, glm::vec3> Spline::operator()(float u)
{
    U = { u * u * u, u * u, u, 1.0 };
    return std::make_pair(MP * U, glm::normalize(MpP * U));
}
