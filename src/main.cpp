// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

#include <iostream>
#include <sstream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm.h>
#include "glRoutine.h"
#include "objLoader.h"
#include "variables.h"
#include "FreeImage.h"

//Eanble High Performance GPU on Optimus devices
extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
using namespace std;

int g_width = 1280;
int g_height = 720;
int g_winId;

objLoader g_meshloader;

void main( int argc, char* argv[] )
{
    //load model
    int numLoadedMesh = 0;
    for( int i = 1; i < argc; ++i )
    {
        string header;
        string data;
        istringstream line(argv[i]);
        getline( line, header, '=' );
        getline( line, data, '=' );
        if( header.compare( "mesh" ) == 0 )
        {
            numLoadedMesh += g_meshloader.load( data );
        }
    }

    if( numLoadedMesh == 0 )
    {
        cout << "Usage: mesh=[obj file]" << endl;
        system("pause");
        return;
    }

    FreeImage_Initialise();

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

    initShader();
    initFBO(g_width, g_height);
    initVertexData(); 
    buildVoxelList(); //build a voxel fragment list
    buildSVO();       //build a sparse voxel octree
    deleteVoxelList();
    octreeTo3Dtex();  //for visualization purpose

    initLight();

    glutMainLoop();

    FreeImage_DeInitialise();
}