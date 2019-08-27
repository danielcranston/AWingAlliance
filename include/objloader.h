#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include <map>

#include <GL/glew.h>



typedef struct {
	unsigned int vbo, vao;
	int numTriangles;
	unsigned int texture_id;
	
	size_t material_id;
	std::string texture_name;
} DrawObject;


struct Model
{
	std::string name;
	std::vector<DrawObject> drawobjects;

	Model() {};
	Model(std::string name, std::vector<DrawObject> dos) : name{name}, drawobjects{dos}	{}

    bool operator==(const Model& m) const
    { 
        return name == m.name;
    } 
};

bool LoadObjAndConvert(float bmin[3], float bmax[3],
                              std::vector<DrawObject>* drawObjects,
                              const char* filename);

void assignTexture(unsigned int& texture_id, const std::string texture_filename);

void loadModels(std::map<std::string, Model>& Models, std::vector<std::string> model_names);
unsigned int loadCubemap(unsigned int vao, const std::vector<std::string>& faces);

void CheckErrors(std::string desc);