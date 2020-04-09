#include <iostream>
#include <memory>
#include <bitset>

#include <skybox.h>
#include <GL/glew.h>

Skybox::Skybox(Model* mdl, unsigned int tex_id, unsigned int program)
  : no_translation(glm::mat4()), texture_id(tex_id), program(program), model(mdl)
{
}

void Skybox::Draw(const glm::mat4& projMatrix, const glm::mat4& camMatrix)
{
    no_translation = camMatrix;
    no_translation[3] = glm::vec4(0, 0, 0, 1);
    const auto mvp = glm::value_ptr(projMatrix * no_translation);

    glDisable(GL_CULL_FACE);
    glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, mvp);
    for (const auto& o : model->drawobjects)
    {
        glBindVertexArray(o->vao);
        glBindTexture(GL_TEXTURE_CUBE_MAP, o->texture_id);
        glDrawArrays(GL_TRIANGLES, 0, 3 * o->numTriangles);
    }
    glEnable(GL_CULL_FACE);
}