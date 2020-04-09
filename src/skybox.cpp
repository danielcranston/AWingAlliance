#include "skybox.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Skybox::Skybox(const unsigned int vao, unsigned int tex_id, unsigned int program)
  : vao(vao), program(program), texture_id(tex_id), no_translation(glm::mat4())
{
}

void Skybox::Draw(const glm::mat4& projMatrix, const glm::mat4& camMatrix) const
{
    no_translation = camMatrix;
    no_translation[3] = glm::vec4(0, 0, 0, 1);
    const auto mvp = projMatrix * no_translation;

    glDisable(GL_CULL_FACE);
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 12);
    glEnable(GL_CULL_FACE);
    glClear(GL_DEPTH_BUFFER_BIT);
}