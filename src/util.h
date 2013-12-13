// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  
#ifndef _UTIL_H
#define _UTIL_H

///This error checking function is from the base code of project 6 of CIS-565 2013 Fall
void checkFramebufferStatus(GLenum framebufferStatus);

GLuint loadTexturFromFile( const char* filename,  GLint internal_format, GLenum format, unsigned int level );

#endif