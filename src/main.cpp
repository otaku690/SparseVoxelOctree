// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

#include  <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm.h>
#include "glRoutine.h"
#include "variables.h"

//Eanble High Performance GPU on Optimus devices
extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
using namespace std;

int g_width = 1280;
int g_height = 720;
int g_winId;

void main( int argc, char* argv[] )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    glutInitContextVersion( 4, 3 );
    glutInitContextFlags( GLUT_FORWARD_COMPATIBLE );
    glutInitContextProfile( GLUT_COMPATIBILITY_PROFILE );

    glutInitWindowSize( g_width, g_height );

    g_winId = glutCreateWindow( "Sparse Voxel Octree" );

    glewInit();

    GLenum err = glewInit();
    if( err != GLEW_OK )
    {
        //glewInit failed, something is seriously wrong
        cout<<"ERROR: glewInit failed.\n";
        exit(1);
    }
    cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;
    cout << "OpenGL version " << glGetString(GL_VERSION) << " supported" << endl;

    glutDisplayFunc( glut_display );
    glutIdleFunc( glut_idle );
    glutReshapeFunc( glut_reshape );
    glutKeyboardFunc( glut_keyboard );
    glutMouseFunc( glut_mouse );
    glutMotionFunc( glut_motion );

    glutMainLoop();

}