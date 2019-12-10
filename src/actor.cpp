#include <iostream>
#include <memory>
#include <bitset>
#include <map>

#include <actor.h>
#include <GL/glew.h>

// 	#define GLM_ENABLE_EXPERIMENTAL
// 	#include <glm/gtx/string_cast.hpp>

extern GLuint program;

Actor::Actor() { }

Actor::Actor(	glm::vec3 p,
				glm::vec3 d,
				std::vector<std::pair<Model*, glm::mat4>> part_vector
				)
	: pos{p}
	, dir{glm::normalize(d)}
{
	for (auto const &p : part_vector)
	{
		parts.push_back({p.first, p.second});
	}
}

void Actor::Draw(glm::mat4 camprojMat)
{
	// Construct Model Matrix from Position and Viewing Direction
	mdlMatrix = glm::inverse(
		glm::lookAt(glm::vec3(0.0, 0.0, 0.0), -dir, glm::vec3(0.0, 1.0, 0.0))
		);
	mdlMatrix = glm::translate(glm::mat4(1.0F), pos) * mdlMatrix; 
	
	// Draw each part
	glm::mat4 mvp = camprojMat * mdlMatrix;
	GLsizei stride = (3 + 3 + 3 + 2) * sizeof(float);
	for(Part p: parts)
	{
		glm::mat4 mvp2 = mvp * p.pose;
		glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp2));
		for(DrawObject o: p.model->drawobjects)
		{
			if(o.texture_name == "")
			{
				glUniform1i(glGetUniformLocation(program, "bUseColor"), 1);
			}
			else
			{
				glUniform1i(glGetUniformLocation(program, "bUseColor"), 0);
			}
			glBindVertexArray(o.vao);
			glBindTexture(GL_TEXTURE_2D, o.texture_id);
			glDrawArrays(GL_TRIANGLES, 0, 3 * o.numTriangles);
		}
	}
}

void Actor::SetPosition(glm::vec3 p)
{
	pos = p;
}