#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <memory>
#include <model.h>
#include <iostream>

#include <GL/glew.h>

struct DrawObject
{
    explicit DrawObject(const std::vector<float>& buffer,
                        const uint tex_id,
                        const std::string& tex_name)
      : vbo(0),
        vao(0),
        numTriangles(buffer.size() / (3 + 3 + 3 + 2) / 3),  // 3:vtx, 3:normal, 3:col, 2:texcoord
        texture_id(tex_id),
        texture_name(tex_name)
    {
        if (buffer.size() > 0)
        {
            // This needs a better home
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(
                GL_ARRAY_BUFFER, buffer.size() * sizeof(float), buffer.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
            GLsizei stride = (3 + 3 + 3 + 2) * sizeof(float);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*)0);
            glVertexAttribPointer(
                1, 3, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * 3));
            glVertexAttribPointer(
                2, 3, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * 6));
            glVertexAttribPointer(
                3, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * 9));
        }
    }
    unsigned int vbo, vao;
    int numTriangles;
    unsigned int texture_id;

    std::string texture_name;
};

struct BoundingBox
{
    unsigned int vao, vbo;
    float xscale, yscale, zscale;
    glm::mat4 pose;

    BoundingBox() {}
    BoundingBox(std::array<float, 3> bmin, std::array<float, 3> bmax)
    {
        xscale = (bmax[0] - bmin[0]) / 2.0;
        yscale = (bmax[1] - bmin[1]) / 2.0;
        zscale = (bmax[2] - bmin[2]) / 2.0;
        glm::vec3 scale = glm::vec3(xscale, yscale, zscale);

        float offset_x = ((bmax[0] + bmin[0]) / 2.0);
        float offset_y = ((bmax[1] + bmin[1]) / 2.0);
        float offset_z = ((bmax[2] + bmin[2]) / 2.0);
        glm::vec3 offset = glm::vec3(offset_x, offset_y, offset_z) / scale;

        pose = glm::translate(glm::scale(glm::mat4(1.0), scale), offset);
    }

};

struct Model
{
    std::string name;
    std::vector<DrawObject> drawobjects;
    BoundingBox boundingbox;

    Model(){};
    Model(std::string name,
          std::vector<DrawObject> dos,
          std::array<float, 3> bmin,
          std::array<float, 3> bmax)
      : name{ name }, drawobjects{ dos }, boundingbox{ BoundingBox(bmin, bmax) }
    {
    }

    bool operator==(const Model& m) const
    { 
        return name == m.name;
    }
};
