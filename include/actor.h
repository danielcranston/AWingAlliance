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
#include <spline.h>

class Actor 
{
	public:
		Actor();
		Actor(	glm::vec3 p,
				glm::vec3 d,
				std::vector<Model*>	mdls,
				std::vector<glm::mat4> poses
				);
		

		virtual void Draw(glm::mat4 camprojMat); // evaluates at runtime
		
		void Update(float speedChange, float turnChange);
		void Update(const std::bitset<8>& keyboardInfo, float dt);
		void Update_Roaming(float t);

		void SetPosition(glm::vec3 pos);
		void PrintStatus();
		
		// std::string name;
		// bool operator==(const Model& m) const
		// { 
		// 	return name == m.name;
		// } 

		struct Part
		{
			Model* model;	// model to draw
			glm::mat4 pose; // pose relative to actor
		};

		Spline s;
		bool bDrawBBox, bDrawSpline;
		glm::vec3 pos;		// positional vector
		glm::vec3 dir;		// directional viewing vector (norm 1)
		std::vector<Part> parts;
	protected:
		glm::mat4 mdlMatrix;


		float throttle; 		// normalized -1 to 1
		float turnThrottle; 	// normalized -1 to 1
		
		float throttleChangeRate;
		float turnThrottleChangeRate;
		
		float maxSpeed;
		float maxTurnSpeed;

		void DrawPart(DrawObject &o, glm::mat4 mvp);
};