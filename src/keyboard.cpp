#include <iostream>

#include <keyboard.h>

char keyStates[256];
bool bUpArrow = false;
bool bDownArrow = false;
bool bLeftArrow = false;
bool bRightArrow = false;

// extern Actor aAWing;


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
	return 0;
}



void ProcessKeyboardInput(std::bitset<8>& flags)
{
	// if ( 'e' == GLUT_KEY_UP ){ std::cout << "They are defined as one and the same..." << '\n'; }
	flags.reset();

	float speedChange = 0;
	float turnChange = 0;

	if ( KeyIsDown('w') ) { speedChange =  0.025; flags.set(W_IS_DOWN); }
	if ( KeyIsDown('s') ) { speedChange =  -0.025; flags.set(S_IS_DOWN); }
	if ( KeyIsDown('q') ) { speedChange =  -0.025; flags.set(Q_IS_DOWN); }

	if (SpecialIsDown(GLUT_KEY_LEFT))	{ turnChange =  0.025; flags.set(LEFTARROW_IS_DOWN); }
	if (SpecialIsDown(GLUT_KEY_RIGHT))	{ turnChange = -0.025; flags.set(RIGHTARROW_IS_DOWN); }
	if (SpecialIsDown(GLUT_KEY_UP))		flags.set(UPARROW_IS_DOWN);
	if (SpecialIsDown(GLUT_KEY_DOWN))	flags.set(DOWNARROW_IS_DOWN);
	if ((KeyIsDown(' ')))				flags.set(SPACEBAR_IS_DOWN);
}