#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <keyboard.h>

namespace
{
char keyStates[256];
bool bUpArrow = false;
bool bDownArrow = false;
bool bLeftArrow = false;
bool bRightArrow = false;
}  // namespace

// glut KeyboardFunc related
void onKeyDown(unsigned char key, int x, int y)
{
    keyStates[key] = true;
}
void onKeyUp(unsigned char key, int x, int y)
{
    keyStates[key] = false;
}
char KeyIsDown(unsigned char c)
{
    return keyStates[c];
}

// glutSpecialFunc related
void onSpecialDown(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_UP:
            bUpArrow = true;
            break;
        case GLUT_KEY_DOWN:
            bDownArrow = true;
            break;
        case GLUT_KEY_LEFT:
            bLeftArrow = true;
            break;
        case GLUT_KEY_RIGHT:
            bRightArrow = true;
            break;
    }
}
void onSpecialUp(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_UP:
            bUpArrow = false;
            break;
        case GLUT_KEY_DOWN:
            bDownArrow = false;
            break;
        case GLUT_KEY_LEFT:
            bLeftArrow = false;
            break;
        case GLUT_KEY_RIGHT:
            bRightArrow = false;
            break;
    }
}
char SpecialIsDown(int key)
{
    switch (key)
    {
        case GLUT_KEY_UP:
            return bUpArrow;
        case GLUT_KEY_DOWN:
            return bDownArrow;
        case GLUT_KEY_LEFT:
            return bLeftArrow;
        case GLUT_KEY_RIGHT:
            return bRightArrow;
    }
    return 0;
}

void ProcessKeyboardInput(std::bitset<8>& flags)
{
    flags.reset();

    float speedChange = 0;
    float turnChange = 0;

    if (KeyIsDown('w'))
    {
        speedChange = 0.025;
        flags.set(KeyboardMapping::W);
    }
    if (KeyIsDown('s'))
    {
        speedChange = -0.025;
        flags.set(KeyboardMapping::S);
    }
    if (KeyIsDown('q'))
    {
        speedChange = -0.025;
        flags.set(KeyboardMapping::Q);
    }

    if (SpecialIsDown(GLUT_KEY_LEFT))
    {
        turnChange = 0.025;
        flags.set(KeyboardMapping::LEFTARROW);
    }
    if (SpecialIsDown(GLUT_KEY_RIGHT))
    {
        turnChange = -0.025;
        flags.set(KeyboardMapping::RIGHTARROW);
    }
    if (SpecialIsDown(GLUT_KEY_UP))
        flags.set(KeyboardMapping::UPARROW);
    if (SpecialIsDown(GLUT_KEY_DOWN))
        flags.set(KeyboardMapping::DOWNARROW);
    if ((KeyIsDown(' ')))
        flags.set(KeyboardMapping::SPACEBAR);
}