#include <iostream>

#include <model.h>
#include <GL/glew.h>

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
