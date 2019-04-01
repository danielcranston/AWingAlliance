#include <actor.h>
#include <iostream>

#define ACTOR_VERBOSE 1

#if ACTOR_VERBOSE
	#define GLM_ENABLE_EXPERIMENTAL
	#include <glm/gtx/string_cast.hpp>
	using std::cout;
#endif

#include <helpfunctions.h>

Actor::Actor() { }
Actor::Actor(glm::vec3 p, glm::vec3 d, std::vector<Model*> mdls, std::vector<GLuint> progs, std::vector<GLuint> texs)
{
	pos = p;
	dir = d;
	speed = 0.0;
	turnspeed = 0.0;

	maxSpeed = 1.0;
	maxTurnSpeed = 1.0;

	for (int i = 0; i < mdls.size(); ++i)
	{
		parts.push_back({ mdls[i], progs[i], texs[i] });
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
	for(Part p: parts)
	{
		DrawPart(p, mvp);
	}
}

void Actor::DrawPart(Actor::Part &p, glm::mat4 mvp)
{
	glUseProgram(p.program);
	glBindVertexArray(p.model->vao);

	// Update location, could be optimized further
	glUniformMatrix4fv(glGetUniformLocation(p.program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
	
	// Texture
	glUniform1i(glGetUniformLocation(p.program, "tex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, p.texture);
	
	// Vertex
	glBindBuffer(GL_ARRAY_BUFFER, p.model->vb);
	glEnableVertexAttribArray(glGetAttribLocation(p.program, "inPosition"));
	glVertexAttribPointer(glGetAttribLocation(p.program, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	// TexCoord
	glBindBuffer(GL_ARRAY_BUFFER, p.model->tb);
	glEnableVertexAttribArray(glGetAttribLocation(p.program, "inTexCoord"));
	glVertexAttribPointer(glGetAttribLocation(p.program, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	// Draw
	glDrawElements(GL_TRIANGLES, p.model->numIndices, GL_UNSIGNED_INT, 0L);
}

void Actor::Update(float speedChange, float turnChange)
{
	// Always strive to reduce Turn Rate
	turnChange = turnChange - 0.5*my_sign(turnspeed)*turnspeed*turnspeed;

	// First update Turn Rate and Viewing Direction
	turnspeed = clamp(turnspeed + turnChange, -1.0, 1.0);
	float turnStep = smootherstep(0.0, 1.0, turnspeed);
	dir = glm::vec3(
			glm::rotate(glm::mat4(1.0f),
						turnStep,
						glm::vec3(0, 1, 0)) *
			glm::vec4(dir, 1.0)
		);

	// Then update Speed and Position
	speed = clamp(speed + speedChange, -1.0, 1.0);
	float speedStep = smootherstep(0.0, 1.0, speed);
	pos += glm::vec3(speedStep * dir);


	#if ACTOR_VERBOSE
	PrintStatus();
	#endif
}

void Actor::SetPosition(glm::vec3 p)
{
	pos = p;
	speed = 0.0;
}

void Actor::PrintStatus()
{

	// std::cout << "mdlMatrix: " << glm::to_string(mdlMatrix) << '\n';
	// std::cout << "camprojMat: " << glm::to_string(camprojMat) << '\n';
	cout << "ACTOR STATUS:" << '\n';
	cout << "  speed: " << speed << '\n';
	cout << "  turnspeed: " << turnspeed << '\n';
	cout << "  dir: " << glm::to_string(dir) << '\n';
	cout << "  pos: " << glm::to_string(pos) << '\n';
}