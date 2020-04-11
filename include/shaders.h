#pragma once

#include <string.h>

char* readFile(const char* file);
void printShaderInfoLog(const uint obj, const char* fn);
uint compileShaders(const std::string& vertexSource, const std::string& fragmentSource);
uint compileComputeShader(const std::string& computeSource);
