#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>

// Use these
char KeyIsDown(unsigned char c); // For ASCII keyboard characters
char SpecialIsDown(int key);     // For Non-ASCII (arrows etc)

void ProcessKeyboardInput();

// Internal
void onKeyDown(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);

void onSpecialDown(int key, int x, int y);
void onSpecialUp(int key, int x, int y);


float smoothstep(float edge0, float edge1, float x);