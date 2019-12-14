#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <parser.h>

class Terrain
{
	public:
		std::vector<float> heightmap;
		std::vector<glm::vec3> normalmap;
		std::vector<unsigned int> indices;
		std::int32_t xSize, zSize, yMax, blockScale;
		unsigned int vao, vboVertices, vboIndices;
		unsigned int program;
		std::vector<unsigned int> texture_ids;
		int numTriangles;

		Terrain();
		Terrain(const ScenarioParser::TerrainEntry &terrainentry, std::map<std::string, uint> *textures, uint program);
		void Generate();
		void PushToGPU();
		void Draw(glm::mat4 camprojMat);
		std::pair<float, glm::vec3> GetHeight(float x, float z);
		float HeightAt(int x, int z);

		bool saveBMP(const std::string& path);
};