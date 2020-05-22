#pragma once

#include <GL/glew.h>
#include <iostream>
#include <vector>

struct FBO
{
  public:
    unsigned int id, texid1, texid2;
    unsigned int width, height;

    explicit FBO(unsigned int width, unsigned int height);
    ~FBO();
};
