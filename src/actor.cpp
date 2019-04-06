#include <actor.h>
#include <iostream>

#if ACTOR_VERBOSE
	#define GLM_ENABLE_EXPERIMENTAL
	#include <glm/gtx/string_cast.hpp>
	using std::cout;
#endif

#include <helpfunctions.h>

extern GLuint colorprogram;

Actor::Actor() { }

Actor::Actor(	glm::vec3 p,
				glm::vec3 d,
				std::vector<Model*> mdls,
				std::vector<glm::mat4> poses,
				std::vector<GLuint> progs,
				std::vector<GLuint> texs,
				//std::vector<std::array<int,3>>  colors
				std::vector<glm::vec3> colors
				)
	: pos{p}, dir{glm::normalize(d)} // Initializer list just because
{
	// pos = p;
	// dir = glm::normalize(d);
	speed = 0.0;
	turnspeed = 0.0;


	maxSpeed = 1.0;
	maxTurnSpeed = 1.0;	

	for (int i = 0; i < mdls.size(); ++i)
	{
		parts.push_back({ 	mdls[i], // Build each part
							poses[i],
							progs[i],
							texs[i],
							colors[i] 
						});
	}
}

void Actor::Draw(glm::mat4 camprojMat)
{
	// Construct Model Matrix from Position and Viewing Direction
	mdlMatrix = glm::inverse(
		glm::lookAt(glm::vec3(0.0, 0.0, 0.0), -dir, glm::vec3(0.00001, 1.0, 0.0001))
		);
	mdlMatrix = glm::translate(glm::mat4(1.0F), pos) * mdlMatrix; 

	// Construct mat4 from (x, y, z) = (forward, )

	//float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * glm::radians(15.0);  // 15° per second
	// glm::mat4 anim =
	// 		glm::rotate(glm::mat4(1.0f), 1*angle, glm::vec3(1, 0, 0)) *
	// 		glm::rotate(glm::mat4(1.0f), 2*angle, glm::vec3(0, 1, 0)) *
	// 		glm::rotate(glm::mat4(1.0f), 3*angle, glm::vec3(0, 0, 1)) *
	// 		glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -1.5));

	
	// Draw each part
	glm::mat4 mvp = camprojMat * mdlMatrix;
	for(Part p: parts)
	{
		glm::mat4 mvp2 = mvp * p.pose;
		DrawPart(p, std::move(mvp2)); // better than copying, worse than passing by ref.
	}
}

void Actor::DrawPart(Actor::Part &p, glm::mat4 mvp)
{
	// Apply this parts relative pose


	glUseProgram(p.program);
	glBindVertexArray(p.model->vao);

	// Update location, could be optimized further
	glUniformMatrix4fv(glGetUniformLocation(p.program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
	
	// Texture / Color
	if (p.program != colorprogram)
	{
		#if ACTOR_VERBOSE
		cout << "  parts texture: " << p.texture << '\n';
		#endif
		glUniform1i(glGetUniformLocation(p.program, "tex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, p.texture);
	}
	else
	{
		#if ACTOR_VERBOSE
		cout << "  parts color: " << glm::to_string(parts[1].color) << '\n';
		if (glGetUniformLocation(p.program, "color") == -1)
			cout << "    Could not bind attribute 'color'! " << '\n';
		#endif
		glUniform3fv(glGetUniformLocation(p.program, "color"), 1, glm::value_ptr(p.color)); // p.color.data() if array
	}
	
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

#if ACTOR_VERBOSE
void Actor::PrintStatus()
{

	// std::cout << "mdlMatrix: " << glm::to_string(mdlMatrix) << '\n';
	// std::cout << "camprojMat: " << glm::to_string(camprojMat) << '\n';
	cout << "ACTOR STATUS:" << '\n';
	// cout << "  speed: " << speed << '\n';
	// cout << "  turnspeed: " << turnspeed << '\n';
	// cout << "  dir: " << glm::to_string(dir) << '\n';
	// cout << "  pos: " << glm::to_string(pos) << '\n';
	// cout << "  color: " << glm::to_string(parts[1].color) << '\n';
}
#endif