// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  
#define GLM_SWIZZLE
#include <gl/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include "glRoutine.h"
#include "variables.h"

using namespace std;
using namespace glm;

mat4 modelview;
mat4 projection;

vec4 eyePos;
vec3 eyeLook;
vec3 upDir;

float FOV = 60.0f;
float zNear = 0.1f;
float zFar = 100.0f;
float aspect;

//Shader

void glut_display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glutSwapBuffers();
}

void glut_idle()
{
    glutPostRedisplay();
}

void glut_reshape( int w, int h )
{
    if( h == 0 || w == 0 )
        return;

    g_width = w;
    g_height = h;

    modelview = glm::lookAt( vec3( eyePos ), eyeLook, upDir );
    projection = glm::perspective( FOV, (float)w/(float)h, zNear, zFar );

}

void glut_mouse( int button, int state, int x, int y )
{
}

void glut_motion( int x, int y )
{
}

void glut_keyboard( unsigned char key, int x, int y )
{
}

void initShader()
{
}

void initVertexData()
{
}