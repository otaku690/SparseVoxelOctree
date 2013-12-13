// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  
#include <gl/glew.h>
#include <iostream>
#include "util.h"
#include "FreeImage.h"

using namespace std;


///This error checking function is from the base code of project 6 of CIS-565 2013 Fall
void checkFramebufferStatus(GLenum framebufferStatus)
{
    switch (framebufferStatus) {
        case GL_FRAMEBUFFER_COMPLETE_EXT: break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
                                          cout<<"Attachment Point Unconnected\n";
                                          break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
                                          cout<<"Missing Attachment\n";
                                          break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                                          cout<<"Dimensions do not match\n";
                                          break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
                                          cout<<"Formats\n";
                                          break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
                                          cout<<"Draw Buffer\n";
                                          break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
                                          cout<<"Read Buffer\n";
                                          break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
                                          cout<<"Unsupported Framebuffer Configuration\n";
                                          break;
        default:
                                          cout<<"Unkown Framebuffer Object Failure\n";
                                          break;
    }
}

GLuint loadTexturFromFile( const char* filename,  GLint internal_format, GLenum format, unsigned int level )
{
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP *dib(0);
    BYTE* bits(0);

    unsigned int w, h;

    fif = FreeImage_GetFileType(filename, 0);
    if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(filename);
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
		return 0;

    if(FreeImage_FIFSupportsReading(fif))
        dib = FreeImage_Load(fif, filename);

    if(!dib)
		return 0;

    bits = FreeImage_GetBits(dib);

	w = FreeImage_GetWidth(dib);
	h = FreeImage_GetHeight(dib);

    if( bits == 0 || w == 0 || h == 0 )
        return 0;

    //texture generation
    GLuint texId = 0;

    glGenTextures( 1, &texId );
    glBindTexture( GL_TEXTURE_2D, texId );
    glTexStorage2D( GL_TEXTURE_2D, level, internal_format, w, h );
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, w, h, format, GL_UNSIGNED_BYTE, bits);

    glGenerateMipmap( GL_TEXTURE_2D );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexImage2D( GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, GL_UNSIGNED_BYTE, bits);
	//Free FreeImage data
	FreeImage_Unload(dib);

    return texId;
}