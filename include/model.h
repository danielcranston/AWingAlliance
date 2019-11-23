#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <model.h>

typedef struct {
	unsigned int vbo, vao;
	int numTriangles;
	unsigned int texture_id;
	
	size_t material_id;
	std::string texture_name;
} DrawObject;

struct BoundingBox
{
	std::vector<glm::vec3> array;
	unsigned int vao, vbo;
	float xscale, yscale, zscale;
	glm::mat4 pose;

	BoundingBox() {}
	BoundingBox(float bmin[3], float bmax[3])
	{
		float scale_x = (bmax[0] - bmin[0]) / 2.0;
		float scale_y = (bmax[1] - bmin[1]) / 2.0;
		float scale_z = (bmax[2] - bmin[2]) / 2.0;
		glm::vec3 scale = glm::vec3(scale_x, scale_y, scale_z);

		float offset_x = ((bmax[0] + bmin[0]) / 2.0);
		float offset_y = ((bmax[1] + bmin[1]) / 2.0);
		float offset_z = ((bmax[2] + bmin[2]) / 2.0);
		glm::vec3 offset = glm::vec3(offset_x, offset_y, offset_z) / scale;

		pose = glm::translate(glm::scale(glm::mat4(1.0), scale), offset);
	}

};

struct Model
{
	std::string name;
	std::vector<DrawObject> drawobjects;
	BoundingBox boundingbox;

	Model() {};
	Model(std::string name, std::vector<DrawObject> dos, float bmin[3], float bmax[3])
		: name{name}
		, drawobjects{dos}
		, boundingbox{BoundingBox(bmin, bmax)}
	{}

    bool operator==(const Model& m) const
    { 
        return name == m.name;
    } 
};