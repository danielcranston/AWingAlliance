#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <objloader.h>
#include <PerlinNoise.hpp>

class Terrain
{
	public:
		std::vector<float> heightmap;
		std::vector<unsigned int> indices;
		std::int32_t xSize, zSize, yMax, blockScale;
		unsigned int vao, vboVertices, vboIndices;
		unsigned int texture_id;
		int numTriangles;

		Terrain();
		Terrain(std::int32_t xsize, std::int32_t ysize, std::int32_t height, std::int32_t blocksize);
		void GenerateHeightMap();
		void PushToGPU();
		void Draw(glm::mat4 camprojMat);
		float GetHeight(float x, float z);

		bool saveBMP(const std::string& path);
};