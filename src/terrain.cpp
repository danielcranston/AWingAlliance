# include <iostream>
# include <fstream>
# include <sstream>

#include <GL/glew.h>

#include <terrain.h>
#include <PerlinNoise.hpp>

Terrain::Terrain() : xSize{64}, zSize{64}, yMax{1}, blockScale{16}, numTriangles(2 * xSize * zSize) {}

Terrain::Terrain(const ScenarioParser::TerrainEntry &terrainentry, std::map<std::string, uint> *textures, uint program)
	: xSize{terrainentry.x}
	, zSize{terrainentry.z}
	, yMax{terrainentry.maxHeight}
	, blockScale{terrainentry.blockSize}
	, numTriangles(2 * xSize * zSize)
	, program{program}
{
	glUseProgram(program);
	texture_ids.reserve(4);
	for(int i = 0; i < 4; i++)
	{
		std::string tex_name = terrainentry.textures[i];
	    texture_ids[i] = textures->find(tex_name)->second;
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
	}

	glUniform1i(glGetUniformLocation(program, "tex0"), 0); // Texture unit 0
	glUniform1i(glGetUniformLocation(program, "tex1"), 1); // Texture unit 1
	glUniform1i(glGetUniformLocation(program, "tex2"), 2); // Texture unit 2
	glUniform1i(glGetUniformLocation(program, "tex3"), 3); // Texture unit 3
	glUniform1f(glGetUniformLocation(program, "maxHeight"), yMax); // to normalize height to [0, 1] in shader
	glActiveTexture(GL_TEXTURE0);

    Generate();
    PushToGPU();
}

void Terrain::Generate()
{
	double frequency = 2; //std::clamp(frequency, 0.1, 64.0);
	int octaves = 4; // std::clamp(octaves, 1, 16);

	const siv::PerlinNoise perlin(1);
	const double fx = xSize / frequency;	
	const double fz = zSize / frequency;

	float Min = std::numeric_limits<float>::max();
	float Max = 0;

	// Heightmap
	for (int z = 0; z < zSize+1; ++z)
	{
		for (int x = 0; x < xSize+1; ++x)
		{
			double h = perlin.octaveNoise0_1(x / fx, z / fz, octaves);

			if(h < Min) Min = h;
			if(h > Max) Max = h;
			heightmap.push_back(h);
		}
	}
	// Scale height to [0, 1]
	for (int i = 0; i < heightmap.size(); ++i)
		heightmap[i] = (heightmap[i] - Min) / (Max - Min); 

	// Indices
	for (int z = 0; z < zSize; ++z)
	{
		for (int x = 0; x < xSize; ++x)
		{
			indices.push_back(z*(xSize+1) + x);
			indices.push_back((z+1)*(xSize+1) + x);
			indices.push_back((z+1)*(xSize+1) + x+1);

			indices.push_back(z*(xSize+1) + x+1);
			indices.push_back((z)*(xSize+1) + x);
			indices.push_back((z+1)*(xSize+1) + x+1);
		}
	}

	// Normalmap
	for (int z = 0; z < zSize+1; ++z)
	{
		for (int x = 0; x < xSize+1; x++)
		{
			glm::vec3 normal = glm::vec3(0.0, 0.0, 0.0);

			float o = HeightAt(x, z);

			// normalized height difference between current and adjacent vertices
			float n = (HeightAt(x, z+1) - o) * blockScale;
			float s = (HeightAt(x, z-1) - o) * blockScale;
			float e = (HeightAt(x+1, z) - o) * blockScale;
			float w = (HeightAt(x-1, z) - o) * blockScale;
			
			float ne = (HeightAt(x+1, z+1) - o) * blockScale;
			float se = (HeightAt(x+1, z-1) - o) * blockScale;
			float nw = (HeightAt(x-1, z+1) - o) * blockScale;
			float sw = (HeightAt(x-1, z-1) - o) * blockScale;
			// std::cout << n << " " << s << " " << e << " " << w << std::endl;
			glm::vec3 normal1 = glm::cross(glm::vec3(0.0, n, 1.0), glm::vec3(1.0, ne, 1.0));
			glm::vec3 normal2 = glm::cross(glm::vec3(1.0, ne, 1.0), glm::vec3(1.0, e, 0.0));

			glm::vec3 normal3 = glm::cross(glm::vec3(1.0, e, 0.0), glm::vec3(0.0, s, -1.0));

			glm::vec3 normal4 = glm::cross(glm::vec3(0.0, s, -1.0), glm::vec3(-1.0, sw, -1.0));
			glm::vec3 normal5 = glm::cross(glm::vec3(-1.0, se, -1.0), glm::vec3(-1.0, w, 0.0));

			glm::vec3 normal6 = glm::cross(glm::vec3(-1.0, w, 0.0), glm::vec3(0.0, n, 1.0));

			normal = glm::normalize( (normal1 + normal2 + normal3 + normal4 + normal5 + normal6));
			assert(normal.y > 0 && normal.y < 1);
			
			normalmap.push_back(normal);
		}
	}

}

float Terrain::HeightAt(int x, int z)
{
	x = (x + xSize) % xSize; // ensure wraparound
	z = (z + zSize) % zSize;
	return yMax * heightmap[z * (xSize+1) + x];
}

void Terrain::PushToGPU()
{
	std::vector<float> buffer;

	for (int z = 0; z < zSize+1; ++z)
	{
		for (int x = 0; x < xSize+1; x++)
		{
			// Vertex Position
			buffer.push_back(blockScale * (-(xSize/2.0) + x));
			buffer.push_back(yMax * (heightmap[z*(xSize+1) + x]));
			buffer.push_back(blockScale * (-(zSize/2.0) + z));

			// Vertex Normal
			glm::vec3 normal = normalmap[z*(xSize+1) + x];
			buffer.push_back(normal.x);
			buffer.push_back(normal.y);
			buffer.push_back(normal.z);

			// Vertex Tex Coord
			buffer.push_back(x * 1.0);
			buffer.push_back(z * 1.0);
		}
	}

	unsigned int stride = (3 + 3 + 2) * sizeof(float);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vboVertices);
	glGenBuffers(1, &vboIndices);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer.at(0), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.at(0), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*)0);
	glVertexAttribPointer(1	, 3, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float)*3));
	glVertexAttribPointer(3	, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float)*6));
}

void Terrain::Draw(glm::mat4 camprojMat)
{
	glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, glm::value_ptr(camprojMat));

	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, texture_ids[0]);
	// glEnable(GL_PRIMITIVE_RESTART);
	
	// glDrawElements(GL_LINE_STRIP, 3 * numTriangles, GL_UNSIGNED_INT, (const void*)0);
	glDrawElements(GL_TRIANGLES, 3 * numTriangles, GL_UNSIGNED_INT, (const void*)0); // 3 * numTriangles

	// glDisable(GL_PRIMITIVE_RESTART);
}

std::pair<float, glm::vec3> Terrain::GetHeight(float x, float z)
{
	float idx_x = (x/blockScale + xSize/2.0);
	float idx_z = (z/blockScale + zSize/2.0);

	int x0 = std::floor(idx_x);
	int z0 = std::floor(idx_z);
	float h0 = HeightAt(x0, z0);

	float interp_x = idx_x - x0; // [0 1]
	float interp_z = idx_z - z0; // [0 1]

	glm::vec3 v_x, v_z, v_temp;
	v_temp = glm::vec3(blockScale, HeightAt(x0+1, z0+1) - h0, blockScale);
	
	if(interp_x < interp_z)
	{
		v_z = glm::vec3(0.0, HeightAt(x0, z0+1) - h0, blockScale);
		v_x = v_temp - v_z;
	}
	else
	{
		v_x = glm::vec3(blockScale, HeightAt(x0+1, z0) - h0, 0.0);
		v_z = v_temp - v_x;

	}
	
	float height = (h0 + interp_x * v_x + interp_z * v_z).y;
	glm::vec3 normal = glm::normalize(glm::cross(v_z, v_x));

	return std::make_pair(height, normal);
}



# pragma pack (push, 1)
struct BMPHeader
{
	std::uint16_t bfType;
	std::uint32_t bfSize;
	std::uint16_t bfReserved1;
	std::uint16_t bfReserved2;
	std::uint32_t bfOffBits;
	std::uint32_t biSize;
	std::int32_t  biWidth;
	std::int32_t  biHeight;
	std::uint16_t biPlanes;
	std::uint16_t biBitCount;
	std::uint32_t biCompression;
	std::uint32_t biSizeImage;
	std::int32_t  biXPelsPerMeter;
	std::int32_t  biYPelsPerMeter;
	std::uint32_t biClrUsed;
	std::uint32_t biClrImportant;
};
static_assert(sizeof(BMPHeader) == 54);
# pragma pack (pop)

static constexpr std::uint8_t ToUint8(double x) noexcept
{
	return x >= 1.0 ? 255 : x <= 0.0 ? 0 : static_cast<std::uint8_t>(x * 255.0 + 0.5);
}

bool Terrain::saveBMP(const std::string& path)
{
	const std::int32_t  rowSize = xSize * 3 + xSize % 4;
	const std::uint32_t bmpsize = rowSize * zSize;
	const BMPHeader header =
	{
		0x4d42,
		static_cast<std::uint32_t>(bmpsize + sizeof(BMPHeader)),
		0,
		0,
		sizeof(BMPHeader),
		40,
		xSize,
		zSize,
		1,
		24,
		0,
		bmpsize,
		0,
		0,
		0,
		0
	};

	if (std::ofstream ofs{ path, std::ios_base::binary })
	{
		ofs.write((const char*)&header, sizeof(header));

		std::vector<std::uint8_t> line(rowSize);

		for (std::int32_t y = zSize - 1; -1 < y; --y)
		{
			size_t pos = 0;

			for (std::int32_t x = 0; x < xSize; ++x)
			{
				const std::uint8_t col = ToUint8(heightmap[y * (xSize+1) + x]);
				line[pos++] = col;
				line[pos++] = col;
				line[pos++] = col;
			}

			ofs.write((const char*)line.data(), line.size());
		}

		return true;
	}
	else
	{
		return false;
	}
};