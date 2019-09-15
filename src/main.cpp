#include <iostream>
#include <math.h>
#include <algorithm>
#include <array>
#include <vector>
#include <memory>
#include <bitset>
#include <map>

/* External Dependencies*/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>

#include <GL/glew.h>
#include <GL/freeglut.h>


#include <actor.h>
#include <keyboard.h>
#include <shaders.h>
#include <objloader.h>
#include <terrain.h>
#include <skybox.h>
#include <fbo.h>
#include <spline.h>


#define EYE glm::mat4(1.0f)
#define SCALE(x) glm::scale(EYE, glm::vec3(x, x, x))
#define SCALE2(x,y,z) glm::scale(EYE, glm::vec3(x, y, z))
#define ROTX(deg) glm::rotate(EYE, glm::radians(deg), glm::vec3(1.0, 0.0, 0.0))
#define ROTY(deg) glm::rotate(EYE, glm::radians(deg), glm::vec3(0.0, 1.0, 0.0))
#define ROTZ(deg) glm::rotate(EYE, glm::radians(deg), glm::vec3(0.0, 0.0, 1.0))
#define TRANS(x, y, z) glm::translate(EYE, glm::vec3(x, y, z))
#define NOCOLOR {0.0,0.0,0.0}
#define UP glm::vec3(0.0, 1.0, 0.0)

uint SCREEN_W, SCREEN_H;
uint timeNow, timeOfLastUpdate, timeOfLastSplineChange;
std::bitset<8> keyboardInfo = 0;

GLuint program, terrainProgram, skyProgram, fboProgram;
glm::mat4 projCamMatrix, camMatrix, projMatrix;

Skybox skybox;
FBO fbo;
Spline s;

std::map<std::string, Model> Models;
std::map<std::string, Actor> Actors;

Actor aAWing, aHangar, aSky, aTie, aBomber, aInterceptor;
Terrain terrain;


void init()
{
	std::cout <<  "init" << '\n';

	SCREEN_W = 1200;
	SCREEN_H = 900;
	timeNow = 0;
	timeOfLastUpdate = 0;
	timeOfLastSplineChange = 0;

	// SHADER STUFF
	program = compileShaders("Shaders/object.vert", "Shaders/object.frag");
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	
	terrainProgram = compileShaders("Shaders/terrain.vert", "Shaders/terrain.frag");

	skyProgram = compileShaders("Shaders/sky.vert", "Shaders/sky.frag");
	glUseProgram(skyProgram);
	glUniform1i(glGetUniformLocation(skyProgram, "tex"), 0);

	fboProgram = compileShaders("Shaders/fbo.vert", "Shaders/fbo.frag");
	glUseProgram(fboProgram);
	glUniform1i(glGetUniformLocation(fboProgram, "screenTexture"), 0);
	glUniform2f(glGetUniformLocation(fboProgram, "scale"), 0.5, 0.5);
	glUniform2f(glGetUniformLocation(fboProgram, "offset"), 0.5, 0.5);
	CheckErrors("setup programs");
	// MODEL STUFF
	std::vector<std::string> model_names = {"cube", "awing", "tie", "tie_bomber", "tie_interceptor", "hangar"};
	loadModels(Models, model_names);
	// ACTOR STUFF
	aAWing = Actor(	{0.0, 112.0, 0.0}, //{16.0, 128.0, 14.0}, //{22.1, -53.0, 69.2},	// Position
					{0.0, 0.0, -1.0},	// Facing direction
					{&Models["awing"]}, //&Models["tie_bomber"], &Models["tie_fixed"], &Models["tie_interceptor"]},	// Model parts
					{EYE}// TRANS(10.0, 0.0, 0.0), TRANS(-10.0, 0.0, 0.0), TRANS(0.0, 0.0, 10.0)},				// Relative orientation
				);

	aTie = Actor(	{0.0, 96.0, -32.0},
					{0.0, 0.0, -1.0},
					{&Models["tie"]},
					{EYE}
				);

	aBomber = Actor({-64.0, 112.0, -64.0},
					{0.0, 0.0, -1.0},
					{&Models["tie_bomber"]},
					{EYE}
				);

	aInterceptor = Actor({-16.0, 96.0, -32.0},
					{0.0, 0.0, -1.0},
					{&Models["tie_interceptor"]},
					{EYE}
				);

	aHangar = Actor({0.0, 192.0, 0.0},	// Position
					{0.0, 0.0, -1.0},	// Facing direction
					{&Models["hangar"]},	// Model parts
					{EYE}				// Relative orientation
				);


	skybox = Skybox(&Models["cube"], "lightblue/512");
	terrain = Terrain(32, 32, 96, 16, terrainProgram); // x, z, maxHeight, blocksize
	// terrain.saveBMP("test.bmp");

	fbo.Init(SCREEN_W, SCREEN_H);
	fbo.SetupQuad();
	CheckErrors("setup fbo");

	s = Spline(aInterceptor.pos, glm::vec3(-64, 112, -64), 1280.0f * aInterceptor.dir, 1280.0f * glm::vec3(0.0, 0.0, -1.0));
	s.UploadPoints();
}

void onIdle()
{
	// float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * glm::radians(15.0);
	// glm::mat4 anim =
	// 			glm::rotate(glm::mat4(1.0f), 1*angle, glm::vec3(1, 0, 0)) *
	// 			glm::rotate(glm::mat4(1.0f), 2*angle, glm::vec3(0, 1, 0)) *
	// 			glm::rotate(glm::mat4(1.0f), 3*angle, glm::vec3(0, 0, 1)) *
	// 			glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -1.5));

	glutPostRedisplay();
}

glm::vec3 randomVec3(float xMax, float yMax, float zMax)
{
	float x = 2 * xMax * (std::rand() / (1.0 * RAND_MAX)) - xMax;
	float y = 2 * yMax * (std::rand() / (1.0 * RAND_MAX)) - yMax;
	float z = 2 * zMax * (std::rand() / (1.0 * RAND_MAX)) - zMax;
	return glm::vec3(x, y, z);
}

void onDisplay()
{
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	timeNow = glutGet(GLUT_ELAPSED_TIME);
	if(timeNow - timeOfLastUpdate > 5)
	{
		ProcessKeyboardInput(keyboardInfo);
		float dt = (timeNow - timeOfLastUpdate) / 1000.0;

		// if(keyboardInfo.test(Q_IS_DOWN)) std::cout << glm::to_string(aAWing.pos) << '\n';
		// std::cout << "Keyboard: " << keyboardInfo << '\n';
	
		aAWing.Update(keyboardInfo, dt);

		aTie.pos.z = -32 - (timeNow / 100.0);
		std::pair<float, glm::vec3> aa = terrain.GetHeight(aTie.pos.x, aTie.pos.z);
		aTie.pos.y = aa.first;
		glm::vec3 right = glm::cross(glm::vec3(aTie.dir.x, 0.0, aTie.dir.z), UP);
		aTie.dir = glm::normalize(glm::cross(aa.second, right));

		if(timeNow - timeOfLastSplineChange > 5000)
		{
			glm::vec3 randDir = glm::normalize(randomVec3(64, 32, 64));
			glm::vec3 randPos = randomVec3(256.0, 64, 256);
			randPos.y = randPos.y + 112;
			s = Spline(aInterceptor.pos, randPos, 1280.0f * aInterceptor.dir, 1280.0f * randDir);
			aBomber.pos = randPos;
			aBomber.dir = randDir;
			timeOfLastSplineChange = timeNow;

		}

		float u = (timeNow - timeOfLastSplineChange) / 5000.0;

		std::pair<glm::vec3, glm::vec3> interp = s(u);
		aInterceptor.pos = interp.first;
		aInterceptor.dir = glm::normalize(interp.second);


		// camMatrix = glm::lookAt(glm::vec3(-0.0, 96.0, 8 + (timeNow / 1000.0)), aAWing.pos, glm::vec3(0.0, 1.0, 0.0));
		camMatrix = glm::lookAt(aAWing.pos - (20.0f * aAWing.dir) + 5.0f * UP, aAWing.pos, glm::vec3(0.0, 1.0, 0.0));

		timeOfLastUpdate = timeNow;
	}


	// Draw to FBO
	camMatrix = glm::lookAt(glm::vec3(-0.0, 96.0, 8 + (timeNow / 1000.0)), aAWing.pos, glm::vec3(0.0, 1.0, 0.0));
	glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
	glViewport(0, 0, fbo.width, fbo.height);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(skyProgram);
	skybox.Draw(projMatrix, camMatrix);
	glClear(GL_DEPTH_BUFFER_BIT);
	CheckErrors("draw sky2");

	projCamMatrix = projMatrix * camMatrix;

	glUseProgram(terrainProgram);
	terrain.Draw(projCamMatrix);
	CheckErrors("draw terrain2");

	glUseProgram(program);
	aAWing.Draw(projCamMatrix);
	aHangar.Draw(projCamMatrix);
	aTie.Draw(projCamMatrix);
	aBomber.Draw(projCamMatrix);
	aInterceptor.Draw(projCamMatrix);
	CheckErrors("draw actors2");

	// Draw to screen
	camMatrix = glm::lookAt(aAWing.pos - (20.0f * aAWing.dir) + 5.0f * UP, aAWing.pos, glm::vec3(0.0, 1.0, 0.0));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glViewport(0, 0, SCREEN_W, SCREEN_H);

	glUseProgram(skyProgram);
	skybox.Draw(projMatrix, camMatrix);
	glClear(GL_DEPTH_BUFFER_BIT);
	CheckErrors("draw sky");
	
	glUseProgram(fboProgram);
	glBindVertexArray(fbo.vao);
	glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, fbo.texid);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_CULL_FACE);
	CheckErrors("draw quad");

	projCamMatrix = projMatrix * camMatrix;

	glUseProgram(terrainProgram);
	terrain.Draw(projCamMatrix);
	CheckErrors("draw terrain");

	glUseProgram(program);
	aAWing.Draw(projCamMatrix);
	aHangar.Draw(projCamMatrix);
	aTie.Draw(projCamMatrix);
	aBomber.Draw(projCamMatrix);
	aInterceptor.Draw(projCamMatrix);
	s.Draw(projCamMatrix);
	CheckErrors("draw actors");


	glutSwapBuffers();

}

void onReshape(int width, int height)
{
	std::cout <<  "onReshape" << '\n';
	SCREEN_W = width;
	SCREEN_H = height;
	glViewport(0, 0, SCREEN_W, SCREEN_H);

	projMatrix = glm::perspective(45.0f, 1.0f*SCREEN_W/SCREEN_H, 0.1f, 8192.0f);
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3,2);
	glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DEPTH); // GLUT_DOUBLE
	glutInitWindowSize(1200, 900);
	glutCreateWindow("awing");

	// glewExperimental = GL_TRUE; 
	
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
    glEnable(GL_PROGRAM_POINT_SIZE);
    // glEnable(GL_PRIMITIVE_RESTART);
	glutMainLoop();

	return 0;
}