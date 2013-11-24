// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

#ifndef _GLROUTINE_H
#define _GLROUTINE_H

//////////////// GLUT callback functions
void glut_display();
void glut_reshape( int w, int h );
void glut_idle();
void glut_mouse( int button, int state, int x, int y );
void glut_motion( int x, int y );
void glut_keyboard( unsigned char key, int x, int y );
////////////////

///Scene voxelization
void voxelizeScene();
////

void initShader();
void initVertexData();
float* createPointCube( int dim );

///3D texture generation
unsigned int gen3DTexture( int dim );

#endif