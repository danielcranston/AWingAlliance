#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <memory>
#include <model.h>
#include <iostream>

#include <GL/glew.h>


struct OBJGroup
{
    explicit OBJGroup(const std::vector<float>& buf,
                      const uint tex_id,
                      const std::string& tex_name)
      : buffer(buf), texture_id(tex_id), texture_name(tex_name)
    {
    }
    const std::vector<float> buffer;
    const std::string texture_name;
    const uint texture_id;
};

struct DrawObject
{
    const int attribs_per_vertex =  (3 + 3 + 3 + 2); // 3:vtx, 3:normal, 3:col, 2:texcoord
    explicit DrawObject(const OBJGroup& group);
    ~DrawObject();
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
    std::vector<std::unique_ptr<DrawObject>> drawobjects;
    BoundingBox boundingbox;

    Model(std::string name,
          std::vector<OBJGroup> groups,
          std::array<float, 3> bmin,
          std::array<float, 3> bmax)
      : name{ name }, boundingbox{ BoundingBox(bmin, bmax) }
    {
        std::cout << "  Creating model:" << std::endl;
        for(const auto& group : groups)
            drawobjects.emplace_back(std::make_unique<DrawObject>(group));
    }


    bool operator==(const Model& m) const
    { 
        return name == m.name;
    }
};
