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

Model::Model(const std::string& name, const std::vector<OBJGroup>& groups, const BoundingBox& bbox)
  : name{ name }, boundingbox{ BoundingBox(bbox) }
{
    std::cout << "  Creating model:" << std::endl;
    for (const auto& group : groups)
        drawobjects.emplace_back(std::make_unique<DrawObject>(group));
}

std::unique_ptr<Model> Model::Create(const std::string& name,
                                     const std::vector<OBJGroup>& groups,
                                     const BoundingBox& bbox)
{
    return std::make_unique<Model>(name, groups, bbox);
}

const BoundingBox& Model::GetBoundingBox() const
{
    return boundingbox;
}

void Model::Draw() const
{
    // When we just want to draw using whatever GL state has already been set up, ignoring the
    // drawobjects textures (eg skybox).
    for (const auto& o : drawobjects)
    {
        glBindVertexArray(o->vao);
        glDrawArrays(GL_TRIANGLES, 0, 3 * o->numTriangles);
    }
}

void Model::DrawWireframe() const
{
    // Same as above, the state is expected to be set up prior to this call.
    for (const auto& o : drawobjects)
    {
        glBindVertexArray(o->vao);
        glDrawArrays(GL_LINE_STRIP, 0, 3 * o->numTriangles);
    }
}

void Model::Draw(const glm::mat4& mvp, const glm::vec3& color, const uint program) const
{
    glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    for (const auto& o : drawobjects)
    {
        if (o->use_color)
        {
            glUniform3f(glGetUniformLocation(program, "uniform_color"), color.x, color.y, color.z);
            glUniform1i(glGetUniformLocation(program, "bUseColor"), 1);
        }
        else
        {
            glUniform1i(glGetUniformLocation(program, "bUseColor"), 0);
        }
        glBindVertexArray(o->vao);
        glBindTexture(GL_TEXTURE_2D, o->texture_id);
        glDrawArrays(GL_TRIANGLES, 0, 3 * o->numTriangles);
    }
}

TerrainModel::TerrainModel(const std::vector<uint>& texture_ids,
                           const std::vector<float>& buffer,
                           const std::vector<uint>& indices,
                           const float max_height,
                           const uint program)
  : texture_ids(texture_ids), num_indices(indices.size())
{
    std::cout << "TerrainModel constructed" << std::endl;
    glUseProgram(program);
    for (std::size_t i = 0; i < texture_ids.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
    }
    glActiveTexture(GL_TEXTURE0);

    glUniform1i(glGetUniformLocation(program, "tex0"), 0);  // Texture unit 0
    glUniform1i(glGetUniformLocation(program, "tex1"), 1);  // Texture unit 1
    glUniform1i(glGetUniformLocation(program, "tex2"), 2);  // Texture unit 2
    glUniform1i(glGetUniformLocation(program, "tex3"), 3);  // Texture unit 3
    glUniform1f(glGetUniformLocation(program, "maxHeight"), max_height);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboVertices);
    glGenBuffers(1, &vboIndices);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer.at(0), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int),
                 &indices.at(0),
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(3);
    unsigned int stride = attribs_per_vertex * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * 3));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * 6));
}

TerrainModel::~TerrainModel()
{
    std::cout << "TerrainModel being destroyed" << std::endl;
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vboVertices);
    glDeleteBuffers(1, &vboIndices);
}

void TerrainModel::Draw(const glm::mat4& mvp, const uint program) const
{
    glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, (const void*)0);
}
