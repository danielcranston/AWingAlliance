#include <iostream>

#include "model.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

OBJGroup::OBJGroup(const std::vector<float>& buf, const uint tex_id, const std::string& tex_name)
  : buffer(buf), texture_id(tex_id), texture_name(tex_name)
{
}

DrawObject::~DrawObject()
{
    std::cout << "DrawObject (" << texture_name << ") being destroyed" << std::endl;
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

DrawObject::DrawObject(const OBJGroup& group)
  : texture_id(group.texture_id),
    texture_name(group.texture_name),
    vao(0),
    vbo(0),
    numTriangles(group.buffer.size() / attribs_per_vertex / 3)
{
    std::cout << "    DrawObject (" << texture_name << ") constructed" << std::endl;

    use_color = texture_name == "" ? 1 : 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER, group.buffer.size() * sizeof(float), group.buffer.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    GLsizei stride = attribs_per_vertex * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * 3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * 6));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * 9));
}

BoundingBox::BoundingBox(const std::array<float, 3>& bmin, const std::array<float, 3>& bmax)
{
    float xscale = (bmax[0] - bmin[0]) / 2.0;
    float yscale = (bmax[1] - bmin[1]) / 2.0;
    float zscale = (bmax[2] - bmin[2]) / 2.0;
    glm::vec3 scale = glm::vec3(xscale, yscale, zscale);

    float offset_x = ((bmax[0] + bmin[0]) / 2.0);
    float offset_y = ((bmax[1] + bmin[1]) / 2.0);
    float offset_z = ((bmax[2] + bmin[2]) / 2.0);
    glm::vec3 offset = glm::vec3(offset_x, offset_y, offset_z) / scale;

    pose = glm::translate(glm::scale(glm::mat4(1.0), scale), offset);
}

Model::Model(const std::string& name,
             const std::vector<OBJGroup>& groups,
             const std::array<float, 3>& bmin,
             const std::array<float, 3>& bmax,
             const ShaderProgram* shad)
  : name{ name }, boundingbox{ BoundingBox(bmin, bmax) }, shader(shad)
{
    std::cout << "  Creating model:" << std::endl;
    for (const auto& group : groups)
        drawobjects.emplace_back(std::make_unique<DrawObject>(group));
}

const DrawObject* Model::get_drawobject(const int i) const
{
    if (i < 0 || i > drawobjects.size())
        throw std::runtime_error("Tried to access element outside vector size");
    return drawobjects[i].get();
}

void Model::Draw(const glm::mat4& mvp, const glm::vec3& color) const
{
    glUniformMatrix4fv(
        glGetUniformLocation(shader->GetProgram(), "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    for (const auto& o : drawobjects)
    {
        if (o->use_color)
        {
            glUniform3f(glGetUniformLocation(shader->GetProgram(), "uniform_color"),
                        color.x,
                        color.y,
                        color.z);
            glUniform1i(glGetUniformLocation(shader->GetProgram(), "bUseColor"), 1);
        }
        else
        {
            glUniform1i(glGetUniformLocation(shader->GetProgram(), "bUseColor"), 0);
        }
        glBindVertexArray(o->vao);
        glBindTexture(GL_TEXTURE_2D, o->texture_id);
        glDrawArrays(GL_TRIANGLES, 0, 3 * o->numTriangles);
    }
}