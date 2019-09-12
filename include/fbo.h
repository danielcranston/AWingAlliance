#include <GL/glew.h>
#include <iostream>
#include <vector>

typedef struct
{
	GLuint texid;
	GLuint fb;
	GLuint rb;
	GLuint depth;
	int width, height;
} FBOstruct;


class FBO
{
public:
	unsigned int id, texid, rbo;
	unsigned int width, height;
	unsigned int vao, vbo;

	void SetupQuad()
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

			std::vector<float> buffer = {	// Vertex pos
											0.0, 0.0,
											1.0, 0.0,
											1.0, 1.0,
											0.0, 0.0,
											0.0, 1.0,
											1.0, 1.0,
											// Tex coords
											0.0, 0.0,
											1.0, 0.0,
											1.0, 1.0,
											0.0, 0.0,
											0.0, 1.0,
											1.0, 1.0
											};
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer.at(0), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			GLsizei stride = 2 * sizeof(float);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (const void*)0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(12 * sizeof(float)));
	}

	void Init(unsigned int SCREEN_W, unsigned int SCREEN_H)
	{
		width = SCREEN_W;
		height = SCREEN_H;
		glGenFramebuffers(1, &id);
		glBindFramebuffer(GL_FRAMEBUFFER, id);

		glGenTextures(1, &texid);
		glBindTexture(GL_TEXTURE_2D, texid);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texid, 0); 


		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);  
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
			std::cout << "FRAMEBUFFER COMPLETE" << '\n';
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


};