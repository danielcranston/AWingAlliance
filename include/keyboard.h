#pragma once
#include <bitset>

// Enum class as a type does not implicitly convert back to its underlying type, so would have to do
// a static_cast (or std::underlying_type?) every time we set the bitset...
struct KeyboardMapping
{
    static const std::size_t UPARROW = 0;
    static const std::size_t DOWNARROW = 1;
    static const std::size_t LEFTARROW = 2;
    static const std::size_t RIGHTARROW = 3;
    static const std::size_t SPACEBAR = 4;
    static const std::size_t W = 5;
    static const std::size_t S = 6;
    static const std::size_t Q = 7;
};

// Use these
char KeyIsDown(unsigned char c);  // For ASCII keyboard characters
char SpecialIsDown(int key);      // For Non-ASCII (arrows etc)
void ProcessKeyboardInput(std::bitset<8>& flags);

// Internal (although need to be exposed outwards because they're used in glutKeyboardFunc)
void onKeyDown(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);
void onSpecialDown(int key, int x, int y);
void onSpecialUp(int key, int x, int y);
