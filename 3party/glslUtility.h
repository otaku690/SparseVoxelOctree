// GLSL Utility: A utility class for loading GLSL shaders, for Patrick Cozzi's CIS565: GPU Computing at the University of Pennsylvania
// Written by Varun Sampath and Patrick Cozzi, Copyright (c) 2012 University of Pennsylvania

#ifndef GLSLUTILITY_H_
#define GLSLUTILITY_H_

#include <cstdlib>

#ifdef __APPLE__
	#include <GL/glfw.h>
#else
	#include <GL/glew.h>
#endif

namespace glslUtility
{

GLuint createProgram(const char *vertexShaderPath, const char *fragmentShaderPath, const char *attributeLocations[], GLuint numberOfLocations);

}
 
#endif
