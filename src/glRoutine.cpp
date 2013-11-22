// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include "glRoutine.h"
#include "variables.h"

using namespace std;
using namespace glm;

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