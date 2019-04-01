#pragma once

#include <vector>
#include <map>

extern "C" {
#include "loadobj.h"
#include "LoadTGA.h"
#include "GL_utilities.h"
}

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Actor 
{
	public:
		Actor();
		Actor(glm::vec3 p, glm::vec3 d, std::vector<Model*> mdls, std::vector<GLuint> progs, std::vector<GLuint> texs);
		

		void Draw(glm::mat4 camprojMat);
		void Update(float speedChange, float turnChange);
		void SetPosition(glm::vec3 pos);
		void PrintStatus();

		struct Part
		{
			Model* model;	// model to draw
			GLuint program;	// shader program to use
			GLuint texture;	// texture to use
		};
	private:
		glm::mat4 mdlMatrix;

		glm::vec3 pos;		// positional vector
		glm::vec3 dir;		// directional viewing vector (norm 1)

		float speed; 		// normalized -1 to 1
		float turnspeed; 	// normalized -1 to 1
		
		float maxSpeed;		// scaled onto speed
		float maxTurnSpeed; // scaled onto turnspeed
		
		std::vector<Part> parts;
		void DrawPart(Part &p, glm::mat4 mvp);
		
		glm::vec3 rightVector; // for cross product calculations
};