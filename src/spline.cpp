#include <spline.h>

Spline::Waypoint::Waypoint(const glm::vec3& pos, const glm::vec3 dir, const float speed)
  : pos(pos), dir(dir), speed(speed)
{
}

Spline::Spline(const Waypoint start, const Waypoint end) : start(start), end(end)
{
    Update();
    approximate_length = EstimateLength();
    approximate_travel_duration = EstimateTravelDuration();
}

void Spline::Update()
{
    const glm::vec3 p0 = start.pos - (start.dir * SPEED_MULTIPLIER * start.speed);
    const glm::vec3 p1 = start.pos;
    const glm::vec3 p2 = end.pos;
    const glm::vec3 p3 = end.pos + (end.dir * SPEED_MULTIPLIER * end.speed);

    const glm::mat4 P = { p0.x, p0.y, p0.z, 1.0, p1.x, p1.y, p1.z, 1.0,
                          p2.x, p2.y, p2.z, 1.0, p3.x, p3.y, p3.z, 1.0 };

    const glm::mat4 M = { -0.5, 3 / 2.0, -3 / 2.0, 0.5, 1.0, -5 / 2.0, 2,   -0.5,
                          -0.5, 0.0,     0.5,      0,   0.0, 1.0,      0.0, 0.0 };

    const glm::mat4 Mp = { 0.0, 0.0,  0.0, 0.0,  -3 / 2.0, 9 / 2.0, -9 / 2.0, 3 / 2.0,
                           2.0, -5.0, 4.0, -1.0, -0.5,     0.0,     0.5,      0.0 };

    MP = P * M;
    MpP = P * Mp;
}

std::pair<glm::vec3, glm::vec3> Spline::operator()(float u) const
{
    U = { u * u * u, u * u, u, 1.0 };
    return std::make_pair(MP * U, glm::normalize(MpP * U));
}

float Spline::GetLength() const
{
    if (!length_calculated)
    {
        approximate_length = EstimateLength();
        length_calculated = true;
    }

    return approximate_length;
}

float Spline::GetTravelDuration() const
{
    if (!travel_duration_calculated)
    {
        approximate_travel_duration = EstimateTravelDuration();
        travel_duration_calculated = true;
    }

    return approximate_travel_duration;
}

float Spline::EstimateLength() const
{
    // There's probably some exact way, but this'll do
    float length = 0;
    glm::vec3 last_pos = this->operator()(0.0f).first;
    for (float u = 0.1f; u <= 1.0f; u += 0.1f)
    {
        glm::vec3 this_pos = this->operator()(u).first;
        length += std::abs(glm::distance(last_pos, this_pos));
        last_pos = this_pos;
    }
    return length;
}

float Spline::EstimateTravelDuration() const
{
    const float length = GetLength();
    const float avg_speed = (start.speed + end.speed) / 2.0f;
    return length / avg_speed;
}
