#include <iostream>

#include <actor.h>

char keyStates[256];
bool bUpArrow = false;
bool bDownArrow = false;
bool bLeftArrow = false;
bool bRightArrow = false;

extern Actor aAWing;;
extern Actor aCoordAxis;
extern Actor aCam;


// glut KeyboardFunc related
void onKeyDown(unsigned char key, int x, int y)
{
    keyStates[(unsigned int)key] = true;
}
void onKeyUp(unsigned char key, int x, int y)
{
    keyStates[(unsigned int)key] = false;
}
char KeyIsDown(unsigned char c)
{
	return keyStates[(unsigned int)c];
}



// glutSpecialFunc related
void onSpecialDown(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_UP    : bUpArrow    = true; break;
	case GLUT_KEY_DOWN  : bDownArrow  = true; break;
	case GLUT_KEY_LEFT  : bLeftArrow  = true; break;
	case GLUT_KEY_RIGHT : bRightArrow = true; break;
	}
}
void onSpecialUp(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_UP    : bUpArrow    = false; break;
	case GLUT_KEY_DOWN  : bDownArrow  = false; break;
	case GLUT_KEY_LEFT  : bLeftArrow  = false; break;
	case GLUT_KEY_RIGHT : bRightArrow = false; break;
	}
}
char SpecialIsDown(int key)
{
	switch(key)
	{
	case GLUT_KEY_UP    : return bUpArrow;
	case GLUT_KEY_DOWN  : return bDownArrow;
	case GLUT_KEY_LEFT  : return bLeftArrow;
	case GLUT_KEY_RIGHT : return bRightArrow;
	}
}



void ProcessKeyboardInput()
{
	// if ( 'e' == GLUT_KEY_UP ){ std::cout << "They are defined as one and the same..." << '\n'; }
	
	float speedChange = 0;
	float turnChange = 0;

	if ( KeyIsDown('w') ) speedChange =  0.025;
	if ( KeyIsDown('s') ) speedChange =  -0.025;

	if (SpecialIsDown(GLUT_KEY_LEFT))	turnChange =  0.025;
	if (SpecialIsDown(GLUT_KEY_RIGHT))	turnChange = -0.025;

	if (KeyIsDown(' ')) aAWing.SetPosition(glm::vec3(0.0));

	aAWing.Update(speedChange, turnChange);

	#if DRAW_OTHERS
	aCam.Update(speedChange, turnChange);
	aCoordAxis.Update(speedChange, turnChange);
	#endif
}