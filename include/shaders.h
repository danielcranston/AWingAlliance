#pragma once

#include <iostream>


#include <GL/glew.h>
#include <GL/freeglut.h>

char* readFile(const char *file);
void printShaderInfoLog(GLuint obj, const char *fn);
GLuint compileShaders(std::string vertexSource, std::string fragmentSource);
