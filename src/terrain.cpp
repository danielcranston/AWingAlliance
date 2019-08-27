# include <iostream>
# include <fstream>
# include <sstream>

#include <terrain.h>

extern GLuint program;

Terrain::Terrain() : xSize{64}, zSize{64}, yMax{1}, blockScale{16}, numTriangles(2 * xSize * zSize) {}

Terrain::Terrain(std::int32_t xsize, std::int32_t zsize, std::int32_t height, std::int32_t blockscale)
	: xSize{xsize}
	, zSize{zsize}
	, yMax{height}
	, blockScale{blockscale}
	// , vao{0}
	// , vboVertices{0}
	// , vboIndices{0}
	, numTriangles(2 * xSize * zSize)
	// , heightmap((xSize+1)*(zSize+1)) // reserves space
	{		
	    glGenTextures(1, &texture_id);
	    glBindTexture(GL_TEXTURE_2D, texture_id);
	    assignTexture(texture_id, "Textures/grass.jpg");

	    GenerateHeightMap();
	    PushToGPU();
	}

void Terrain::GenerateHeightMap()
{
	double frequency = 2; //std::clamp(frequency, 0.1, 64.0);
	int octaves = 4; // std::clamp(octaves, 1, 16);

	const siv::PerlinNoise perlin(1);
	const double fx = xSize / frequency;	
	const double fz = zSize / frequency;

	float Min = std::numeric_limits<float>::max();
	float Max = 0;

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
}

void CheckErrors2(std::string desc) {
	GLenum e = glGetError();
	if (e != GL_NO_ERROR) {
		fprintf(stderr, "OpenGL error in \"%s\": %d (%d)\n", desc.c_str(), e, e);
		exit(20);
	}
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
			buffer.push_back(yMax * (heightmap[z*(xSize+1) + x])); // CHECK
			buffer.push_back(blockScale * (-(zSize/2.0) + z));

			// Vertex Tex Coord
			buffer.push_back(x * 1.0);
			buffer.push_back(z * 1.0);
		}
	}

	unsigned int stride = (3 + 2) * sizeof(float);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vboVertices);
	glGenBuffers(1, &vboIndices);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer.at(0), GL_STATIC_DRAW);
	CheckErrors2("vbo vertices");

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
	CheckErrors2("vbo indices");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.at(0), GL_STATIC_DRAW);
	CheckErrors2("vbo indices2");
	std::cout << "  done" << '\n';


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*)0);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float)*3));
}

void Terrain::Draw(glm::mat4 camprojMat)
{
	glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, glm::value_ptr(camprojMat));
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glBindVertexArray(vao);
	// glEnable(GL_PRIMITIVE_RESTART);
	
	// glDrawElements(GL_LINE_STRIP, 3 * numTriangles, GL_UNSIGNED_INT, (const void*)0);
	glDrawElements(GL_TRIANGLES, 3 * numTriangles, GL_UNSIGNED_INT, (const void*)0); // 3 * numTriangles

	// glDisable(GL_PRIMITIVE_RESTART);
}

float Terrain::GetHeight(float x, float z)
{
	float idx_x = (x/blockScale + xSize/2.0);
	float idx_z = (z/blockScale + zSize/2.0);
	std::cout << "idx_x=" << idx_x << '\n';
	std::cout << "idx_z=" << idx_z << '\n';

	int x0 = std::floor(idx_x);
	int x1 = x0 + 1;
	int z0 = std::floor(idx_z);
	int z1 = z0 + 1;

	float v1, v2, v3;
	if(idx_x - x0 > idx_z - z0)
	{
		v1 = heightmap[z0 * xSize + x0];
		v2 = heightmap[z0 * xSize + x1];
		v3 = heightmap[z1 * xSize + x0];
	}
	else
	{
		v1 = heightmap[z0 * xSize + x0];
		v2 = heightmap[z1 * xSize + x1];
		v3 = heightmap[z1 * xSize + x1];	
	}

	float dir1 = v1 - v2;
	float dir2 = v1 - v3;

	
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