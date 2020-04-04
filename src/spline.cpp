#include <spline.h>

glm::vec3 randomVec3(float xMax, float yMax, float zMax)
{
    float x = 2 * xMax * (std::rand() / (1.0 * RAND_MAX)) - xMax;
    float y = 2 * yMax * (std::rand() / (1.0 * RAND_MAX)) - yMax;
    float z = 2 * zMax * (std::rand() / (1.0 * RAND_MAX)) - zMax;
    return glm::vec3(x, y, z);
}

Spline::Spline() {}

Spline::Spline(glm::vec3 p_start, glm::vec3 dir_start, float start_t, float dur)
{
    start_time = start_t;
    duration = dur;
    arrive_time = start_t + duration;


    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glm::vec3 p_end = randomVec3(256.0, 32, 256);
    glm::vec3 dir_end = glm::normalize(randomVec3(64, 32, 64));
    p_end.y = p_end.y + 128;
    Update(p_start, dir_start, p_end, dir_end);

    UploadPoints();
}

void Spline::UpdateRandom(glm::vec3 p_start, glm::vec3 dir_start)
{
    start_time = arrive_time;
    arrive_time = arrive_time + duration;

    glm::vec3 p_end = randomVec3(256.0, 32, 256);
    glm::vec3 dir_end = glm::normalize(randomVec3(64, 32, 64));
    p_end.y = p_end.y + 128;
    Update(p_start, 1024.0f * dir_start, p_end, 1024.0f * dir_end);
}

void Spline::Update(glm::vec3 p_start, glm::vec3 dir_start, glm::vec3 p_end, glm::vec3 dir_end)
{
    p0 = p_start - dir_start;
    p1 = p_start;
    p2 = p_end;
    p3 = p_end + dir_end;

    glm::mat4 P = {p0.x, p0.y, p0.z, 1.0,
                    p1.x, p1.y, p1.z, 1.0,
                    p2.x, p2.y, p2.z, 1.0,
                    p3.x, p3.y, p3.z, 1.0};

    glm::mat4 M = {-0.5, 3/2.0, -3/2.0, 0.5,
                    1.0, -5/2.0, 2, -0.5,
                    -0.5, 0.0, 0.5, 0,
                    0.0, 1.0, 0.0, 0.0};

    glm::mat4 Mp = {0.0, 0.0, 0.0, 0.0,
                    -3/2.0, 9/2.0, -9/2.0, 3/2.0,
                    2.0, -5.0, 4.0, -1.0, 
                    -0.5, 0.0, 0.5, 0.0};

    MP = P * M;
    MpP = P * Mp;
}

std::pair<glm::vec3, glm::vec3> Spline::operator () (float u)
{
    U = {u*u*u, u*u, u, 1.0};
    return std::make_pair(MP * U, glm::normalize(MpP*U));
}

void Spline::UploadPoints()
{
    std::vector<float> buffer;
    for(int i = 0; i < 16; i++)
    {
        std::pair<glm::vec3, glm::vec3> loc = (*this)(i / 15.0);
        buffer.push_back(loc.first.x);
        buffer.push_back(loc.first.y);
        buffer.push_back(loc.first.z);
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer.at(0), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);
}

void Spline::Draw(glm::mat4 camprojMatrix)
{
    glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, glm::value_ptr(camprojMatrix));
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDrawArrays(GL_LINE_STRIP, 0, 16);
}