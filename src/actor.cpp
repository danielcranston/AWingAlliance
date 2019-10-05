#include <iostream>
#include <memory>
#include <bitset>

#include <actor.h>
#include <map>
#include <helpfunctions.h>
#include <keyboard_constants.h>
#include <spline.h>

// 	#define GLM_ENABLE_EXPERIMENTAL
// 	#include <glm/gtx/string_cast.hpp>
// 	using std::cout;

extern GLuint skyProgram;
extern GLuint program;
extern std::map<std::string, Model> Models;

Actor::Actor() { }

Actor::Actor(	glm::vec3 p,
				glm::vec3 d,
				std::vector<Model*>	mdls,
				std::vector<glm::mat4> poses
				)
	: pos{p}
	, dir{glm::normalize(d)}
	, bDrawBBox(false)
	, bDrawSpline(false)
{
	throttle = 0.0;
	turnThrottle = 0.0;

	maxSpeed = 50.0;
	maxTurnSpeed = 1.0 * glm::pi<float>();

	throttleChangeRate = 0.001;
	turnThrottleChangeRate = 0.001;

	s = Spline(p, dir, 0.0, 5.0);

	for (int i = 0; i < mdls.size(); ++i)
	{
		parts.push_back({ 	mdls[i], // Build each part
							poses[i],
						});
	}
}

void Actor::Update_Roaming(float t)
{
	if(t - s.start_time > s.duration)
	{
		s.UpdateRandom(pos,	dir);
		s.UploadPoints();
	}

	float u = (t - s.start_time) / s.duration; // [0, 1]
	std::pair<glm::vec3, glm::vec3> interp = s(u);
	pos = interp.first;
	dir = glm::normalize(interp.second);
}

void Actor::Draw(glm::mat4 camprojMat)
{
	// Construct Model Matrix from Position and Viewing Direction
	mdlMatrix = glm::inverse(
		glm::lookAt(glm::vec3(0.0, 0.0, 0.0), -dir, glm::vec3(0.0, 1.0, 0.0))
		);
	mdlMatrix = glm::translate(glm::mat4(1.0F), pos) * mdlMatrix; 

	//float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * glm::radians(15.0);  // 15° per second
	// glm::mat4 anim =
	// 		glm::rotate(glm::mat4(1.0f), 1*angle, glm::vec3(1, 0, 0)) *
	// 		glm::rotate(glm::mat4(1.0f), 2*angle, glm::vec3(0, 1, 0)) *
	// 		glm::rotate(glm::mat4(1.0f), 3*angle, glm::vec3(0, 0, 1)) *
	// 		glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -1.5));

	
	// Draw each part
	glm::mat4 mvp = camprojMat * mdlMatrix;
	GLsizei stride = (3 + 3 + 3 + 2) * sizeof(float);
	for(Part p: parts)
	{
		glm::mat4 mvp2 = mvp * p.pose;
		glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp2));
		for(DrawObject o: p.model->drawobjects)
		{
			glBindVertexArray(o.vao);
			// if(p.program == skyProgram) {glBindTexture(GL_TEXTURE_CUBE_MAP, o.texture_id);} //glBindTexture(GL_TEXTURE_2D, 0); 
			glBindTexture(GL_TEXTURE_2D, o.texture_id);
			glDrawArrays(GL_TRIANGLES, 0, 3 * o.numTriangles);

		}
		// Draw parts bounding box
		if(bDrawBBox)
		{
			glm::mat4 mvp2 = mvp * p.model->boundingbox.pose;
			glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp2));
			glBindVertexArray(Models["cube"].drawobjects[0].vao);
			glDrawArrays(GL_LINE_STRIP, 0, 3 * 12);
		}

		if(bDrawSpline)
		{
			s.Draw(camprojMat);
		}
	}
}


void Actor::Update(const std::bitset<8>& keyboardInfo, float dt)
{
	glm::vec3 rightVector = glm::cross(dir, glm::vec3(0.0, 1.0, 0.0));

	// std::cout << "dt: " << dt << '\n';
	if(keyboardInfo.test(LEFTARROW_IS_DOWN))
	{
		dir = glm::vec3(
				glm::rotate(glm::mat4(1.0f), maxTurnSpeed * dt, glm::vec3(0, 1, 0))
				* glm::vec4(dir, 1.0));
	}
	if(keyboardInfo.test(RIGHTARROW_IS_DOWN))
	{
		dir = glm::vec3(
				glm::rotate(glm::mat4(1.0f), -maxTurnSpeed * dt, glm::vec3(0, 1, 0))
				* glm::vec4(dir, 1.0));
	}
	if(keyboardInfo.test(UPARROW_IS_DOWN) && dir.y > -0.9)
	{
		dir = glm::vec3(
				glm::rotate(glm::mat4(1.0f), -maxTurnSpeed * dt, rightVector)
				* glm::vec4(dir, 1.0));
	}
	if(keyboardInfo.test(DOWNARROW_IS_DOWN) && dir.y < 0.9)
		{
			dir = glm::vec3(
					glm::rotate(glm::mat4(1.0f), maxTurnSpeed * dt, rightVector)
					* glm::vec4(dir, 1.0));
		}

	if(keyboardInfo.test(W_IS_DOWN)) pos += glm::vec3(maxSpeed * dt * dir);
	if(keyboardInfo.test(S_IS_DOWN)) pos -= glm::vec3(maxSpeed * dt * dir);
	if(keyboardInfo.test(SPACEBAR_IS_DOWN)) pos = glm::vec3(0.0);

}

void Actor::SetPosition(glm::vec3 p)
{
	pos = p;
	throttle = 0.0;

}