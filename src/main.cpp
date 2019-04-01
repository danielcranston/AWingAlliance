#include <iostream>
#include <math.h>
#include <algorithm>

/* External Dependencies*/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #define GLM_ENABLE_EXPERIMENTAL
// #include <glm/gtx/string_cast.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>

/* Ragnenalm Dependencies */
extern "C" {
#include "loadobj.h"
#include "LoadTGA.h"
#include "GL_utilities.h"
}

#include <actor.h>
#include <keyboard.h>

GLuint texAWing;
GLuint program, colorprogram;
Model *mAWing1, *mAWing2;
Actor aActor;
glm::mat4 projCamMatrix, camMatrix, projMatrix;

uint SCREEN_W, SCREEN_H;

void init()
{
	SCREEN_W = 1200;
	SCREEN_H = 900;

	mAWing1 = LoadModelPlus("../Models/awing1.obj");
	mAWing2 = LoadModelPlus("../Models/awing2.obj");

	// Already taken care of by LoadModelPlus!
	// glBindBuffer(GL_ARRAY_BUFFER, mAWing1->vb);
	// glBufferData(GL_ARRAY_BUFFER, mAWing1->numVertices*3*sizeof(GLfloat), mAWing1->vertexArray, GL_STATIC_DRAW);
	// glBindBuffer(GL_ARRAY_BUFFER, mAWing1->tb);
	// glBufferData(GL_ARRAY_BUFFER, mAWing1->numVertices*2*sizeof(GLfloat), mAWing1->texCoordArray, GL_STATIC_DRAW);

	// glBindBuffer(GL_ARRAY_BUFFER, mAWing2->vb);
	// glBufferData(GL_ARRAY_BUFFER, mAWing2->numVertices*3*sizeof(GLfloat), mAWing2->vertexArray, GL_STATIC_DRAW);


	program = loadShaders("../Shaders/object.vert", "../Shaders/object.frag");
	glUseProgram(program);

	// Vertex Buffer
	glEnableVertexAttribArray(glGetAttribLocation(program, "inPosition"));
	glVertexAttribPointer(glGetAttribLocation(program, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

	// TexCoord Buffer
	glEnableVertexAttribArray(glGetAttribLocation(program, "inTexCoord"));
	glVertexAttribPointer(glGetAttribLocation(program, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	//Texture Loading and Uploading to GPU
	LoadTGATextureSimple("../Textures/A-Wing_Diff.tga", &texAWing);

	// Texture Binding
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texAWing);

	/* Color program */
	colorprogram = loadShaders("../Shaders/object.vert", "../Shaders/color.frag");
	glUseProgram(colorprogram);
	glEnableVertexAttribArray(glGetAttribLocation(colorprogram, "inPosition"));
	glVertexAttribPointer(glGetAttribLocation(colorprogram, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Create AWing Actor
	aActor = Actor(	{0.0, 0.0, 0.0},		// Position
					{0.0, 0.0, -1.0},		// Facing Direction
					{mAWing1, mAWing2},		// Model Parts
					{program, colorprogram},// shaders to use to draw them
					{texAWing, 0}			// textures to draw them with
				);


	camMatrix = glm::lookAt(glm::vec3(0.0, 10.0, 20.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
}


void onIdle()
{
	// float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * glm::radians(15.0);
	// glm::mat4 anim =
	// 			glm::rotate(glm::mat4(1.0f), 1*angle, glm::vec3(1, 0, 0)) *
	// 			glm::rotate(glm::mat4(1.0f), 2*angle, glm::vec3(0, 1, 0)) *
	// 			glm::rotate(glm::mat4(1.0f), 3*angle, glm::vec3(0, 0, 1)) *
	// 			glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -1.5));

	ProcessKeyboardInput();

	glutPostRedisplay();
}

void onDisplay()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	aActor.Draw(projCamMatrix);

	glutSwapBuffers();
}

void onReshape(int width, int height)
{
	SCREEN_W = width;
	SCREEN_H = height;
	glViewport(0, 0, SCREEN_W, SCREEN_H);

	projMatrix = glm::perspective(45.0f, 1.0f*SCREEN_W/SCREEN_H, 0.1f, 400.0f);
	projCamMatrix = projMatrix * camMatrix;
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3,2);
	glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(1200, 900);
	glutCreateWindow("AWingAlliance");

	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return EXIT_FAILURE;
	}
	else
	{
		fprintf(stderr, "GLEW Initialized: %s\n", glewGetErrorString(glew_status));
	}

	init();
	

	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
    glutReshapeFunc(onReshape);
	glutIdleFunc(onIdle);
	glutDisplayFunc(onDisplay);
    glutKeyboardFunc(onKeyDown);
    glutKeyboardUpFunc(onKeyUp);
    glutSpecialFunc(onSpecialDown);
    glutSpecialUpFunc(onSpecialUp);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	glutMainLoop();

	return 0;
}