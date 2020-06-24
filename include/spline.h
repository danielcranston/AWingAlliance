#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <utility>
#include <GL/glew.h>
#include <iostream>

// extern GLuint program;

class Spline
{
  public:
    struct Waypoint
    {
        explicit Waypoint(const glm::vec3& pos, const glm::vec3 dir, const float speed);
        glm::vec3 pos;
        glm::vec3 dir;
        float speed;
    };
    explicit Spline(const Waypoint start, const Waypoint end);

    std::pair<glm::vec3, glm::vec3> operator()(float u) const;
    float GetLength() const;
    float GetTravelDuration() const;

  private:
    Waypoint start;
    Waypoint end;

    glm::mat4 MP, MpP;
    mutable glm::vec4 U;

    void Update();
    float EstimateLength() const;
    float EstimateTravelDuration() const;

    // Used to "drag out" the 0th and 3rd control point to make smoother trajectory
    static constexpr float SPEED_MULTIPLIER = 10.0f;

    mutable bool length_calculated = false;
    mutable bool travel_duration_calculated = false;
    mutable float approximate_length;
    mutable float approximate_travel_duration;
};
