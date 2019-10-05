#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <objloader.h>

class Skybox
{
	public: 
		Skybox();
		Skybox(Model* mdl, std::string folder);
		void Draw(const glm::mat4 &projMatrix, const glm::mat4 &camMatrix);
		void LoadAndSetTexture(std::string folder);
		glm::mat4 no_translation;
		Model* model;
	private:
		unsigned int texture_id;
};