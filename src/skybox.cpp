#include <iostream>
#include <memory>
#include <bitset>

#include <skybox.h>

// 	#define GLM_ENABLE_EXPERIMENTAL
// 	#include <glm/gtx/string_cast.hpp>
// 	using std::cout;

extern GLuint skyProgram;

Skybox::Skybox() {}

Skybox::Skybox(Model* mdl, std::string folder)
{
	model = mdl;
	no_translation = glm::mat4();

	LoadAndSetTexture(folder);
}

void Skybox::LoadAndSetTexture(std::string folder)
{
	std::vector<std::string> faces = {
        "Textures/skybox/" + folder + "/right.png",
        "Textures/skybox/" + folder + "/left.png",
        "Textures/skybox/" + folder + "/top.png",
        "Textures/skybox/" + folder + "/bot.png",
        "Textures/skybox/" + folder + "/front.png",
        "Textures/skybox/" + folder + "/back.png"
    };

    texture_id = loadCubemap(model->drawobjects[0].vao, faces);
    model->drawobjects[0].texture_id = texture_id;
}

void Skybox::Draw(const glm::mat4 &projMatrix, const glm::mat4 &camMatrix)
{
	no_translation = camMatrix;
	no_translation[3] = glm::vec4(0, 0, 0, 1);


	glDisable(GL_CULL_FACE);
	glUniformMatrix4fv(glGetUniformLocation(skyProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(projMatrix * no_translation));
	for(DrawObject o: model->drawobjects)
	{
		glBindVertexArray(o.vao);
		glBindTexture(GL_TEXTURE_CUBE_MAP, o.texture_id);
		glDrawArrays(GL_TRIANGLES, 0, 3 * o.numTriangles);
	}
	glEnable(GL_CULL_FACE);

}