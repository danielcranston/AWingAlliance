#pragma once

#include <vector>
#include <map>
#include <memory>
#include <bitset>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include <model.h>

class Actor 
{
	public:
		Actor();
		Actor(	glm::vec3 p,
				glm::vec3 d,
				std::vector<std::pair<Model*, glm::mat4>> part_vector
				);

		virtual void Draw(glm::mat4 camprojMat); // evaluates at runtime
		void SetPosition(glm::vec3 pos);
		void SetDirection(glm::vec3 dir);
		void SetOrientation(glm::vec3 pos, glm::vec3 dir);
		
		struct Part
		{
			Part(Model* m, glm::mat4 p) : model(m), pose(p) {}
			Model* model;	// model to draw
			glm::mat4 pose; // pose relative to actor
		};

		glm::vec3 pos;		// positional vector
		glm::vec3 dir;		// directional viewing vector (norm 1)
		std::vector<Part> parts;
	protected:
		glm::mat4 mdlMatrix;

		void DrawPart(DrawObject &o, glm::mat4 mvp);
};