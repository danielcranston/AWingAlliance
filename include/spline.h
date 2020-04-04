#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <utility>
#include <GL/glew.h>
#include <iostream>

extern GLuint program;

class Spline
{
public:
    glm::vec3 p0, p1, p2, p3;
    glm::mat4 MP,MpP;
    glm::vec4 U;
    unsigned int vao, vbo;
    float start_time, duration, arrive_time;

    Spline();
    Spline(glm::vec3 p_start, glm::vec3 dir_start, float start_t, float dur);

    void Update(glm::vec3 p_start, glm::vec3 dir_start, glm::vec3 p_end, glm::vec3 dir_end);
    void UpdateRandom(glm::vec3 p_start, glm::vec3 dir_start);

    std::pair<glm::vec3, glm::vec3> operator () (float u);

    void UploadPoints();
    void Draw(glm::mat4 camprojMatrix);
};
