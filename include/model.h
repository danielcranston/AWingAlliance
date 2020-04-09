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
                      const std::string& tex_name);

    const std::vector<float> buffer;
    const std::string texture_name;
    const uint texture_id;
};

struct DrawObject
{
    explicit DrawObject(const OBJGroup& group);
    ~DrawObject();

    const int attribs_per_vertex = (3 + 3 + 3 + 2);  // 3:vtx, 3:normal, 3:col, 2:texcoord

    unsigned int vbo, vao;
    int numTriangles;
    bool use_color;
    unsigned int texture_id;
    std::string texture_name;
};

struct BoundingBox
{
    explicit BoundingBox(const std::array<float, 3>& bmin, const std::array<float, 3>& bmax);

    glm::mat4 pose;
};

class Model
{
  public:
    explicit Model(const std::string& name,
                   const std::vector<OBJGroup>& groups,
                   const std::array<float, 3>& bmin,
                   const std::array<float, 3>& bmax);

    void Draw(const glm::mat4& camprojMat, const uint program, const glm::vec3& color) const;

    // until I clean up skybox these will remain public
    std::string name;
    std::vector<std::unique_ptr<DrawObject>> drawobjects;
    BoundingBox boundingbox;
};
