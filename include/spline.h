#include <glm/glm.hpp>
#include<vector>
#include <utility>

class Spline
{
public:
	glm::vec3 p0, p1, p2, p3;
	glm::mat4 MP,MpP;
	glm::vec4 U;

	Spline() {}
	Spline(glm::vec3 p_start, glm::vec3 p_end, glm::vec3 dir_start, glm::vec3 dir_end)
	{
		p0 = p_start - dir_start;
		p1 = p_start;
		p2 = p_end;
		p3 = p_end + dir_end;

		glm::mat4 P = {p0.x, p0.y, p0.z, 1.0,
						p1.x, p1.y, p1.z, 1.0,
						p2.x, p2.y, p2.z, 1.0,
						p3.x, p3.y, p3.z, 1.0};

		glm::mat4 M = {-0.5, 3/2.0, -3/2.0, 0.5,
						1.0, -5/2.0, 2, -0.5,
						-0.5, 0.0, 0.5, 0,
						0.0, 1.0, 0.0, 0.0};

		glm::mat4 Mp = {0.0, 0.0, 0.0, 0.0,
						-3/2.0, 9/2.0, -9/2.0, 3/2.0,
						2.0, -5.0, 4.0, -1.0, 
						-0.5, 0.0, 0.5, 0.0};

		MP = P * M;
		MpP = P * Mp;
	}
	std::pair<glm::vec3, glm::vec3> operator () (float u)
	{
		U = {u*u*u, u*u, u, 1.0};

		return std::make_pair(MP * U, MpP*U);
	}

	std::vector<float> GetPoints()
	{
		std::vector<float> buffer;
		for(int i = 0; i < 10; i++)
		{
			
		}
	}
};