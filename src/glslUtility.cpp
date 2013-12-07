// GLSL Utility: A utility class for loading GLSL shaders, for Patrick Cozzi's CIS565: GPU Computing at the University of Pennsylvania
// Written by Varun Sampath and Patrick Cozzi, Copyright (c) 2012 University of Pennsylvania
// Modified by Cheng-Tso Lin to accommodate geometry shader and compute shader creation

#include "glslUtility.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

namespace glslUtility {

	char* loadFile(const char *fname, GLint &fSize)
	{
		ifstream::pos_type size;
		char * memblock;
		std::string text;

		// file read based on example in cplusplus.com tutorial
		ifstream file (fname, ios::in|ios::binary|ios::ate);
		if (file.is_open())
		{
			size = file.tellg();
			fSize = (GLuint) size;
			memblock = new char [size];
			file.seekg (0, ios::beg);
			file.read (memblock, size);
			file.close();
			cout << "file " << fname << " loaded" << endl;
			text.assign(memblock);
		}
		else
		{
			cout << "Unable to open file " << fname << endl;
			exit(1);
		}
		return memblock;
	}

	// printShaderInfoLog
	// From OpenGL Shading Language 3rd Edition, p215-216
	// Display (hopefully) useful error messages if shader fails to compile
	void printShaderInfoLog(GLint shader)
	{
		int infoLogLen = 0;
		int charsWritten = 0;
		GLchar *infoLog;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

		// should additionally check for OpenGL errors here

		if (infoLogLen > 0)
		{
			infoLog = new GLchar[infoLogLen];
			// error check for fail to allocate memory omitted
			glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
			cout << "InfoLog:" << endl << infoLog << endl;
			delete [] infoLog;
		}

		// should additionally check for OpenGL errors here
	}

	void printLinkInfoLog(GLint prog) 
	{
		int infoLogLen = 0;
		int charsWritten = 0;
		GLchar *infoLog;

		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

		// should additionally check for OpenGL errors here

		if (infoLogLen > 0)
		{
			infoLog = new GLchar[infoLogLen];
			// error check for fail to allocate memory omitted
			glGetProgramInfoLog(prog,infoLogLen, &charsWritten, infoLog);
			cout << "InfoLog:" << endl << infoLog << endl;
			delete [] infoLog;
		}
	}

    GLuint initshaders (GLenum type, const char *filename) 
    {
        GLuint shader = glCreateShader(type) ; 
        GLint compiled ; 
        char *ss;
        GLint slen;

        ss = loadFile(filename,slen);
        const char * cs = ss;

        glShaderSource (shader, 1, &cs, &slen) ; 
        glCompileShader (shader) ; 
        glGetShaderiv (shader, GL_COMPILE_STATUS, &compiled) ; 
		if (!compiled)
		{
			cout << "Shader not compiled." << endl;
			printShaderInfoLog(shader);
		} 
        delete [] ss;

        return shader ; 
    }

	shaders_t loadShaders(const char * vert_path, const char * frag_path, const char * geom_path, const char * compute_path) {
		GLuint f = 0, v = 0, g = 0, c = 0;
        
        if( vert_path )
		    v = initshaders( GL_VERTEX_SHADER, vert_path );
        if( frag_path )
            f = initshaders( GL_FRAGMENT_SHADER, frag_path );
        if( geom_path )
            g = initshaders( GL_GEOMETRY_SHADER, geom_path );
        if( compute_path )
            c = initshaders( GL_COMPUTE_SHADER, compute_path );

        shaders_t out; out.vertex = v; out.fragment = f; out.geometry = g;
        out.compute = c;

		return out;
	}

	void attachAndLinkProgram( GLuint program, shaders_t shaders) 
    {
        if( shaders.vertex )
            glAttachShader(program, shaders.vertex);
        if( shaders.fragment )
		    glAttachShader(program, shaders.fragment);
        if( shaders.geometry )
            glAttachShader(program, shaders.geometry  );
        if( shaders.compute)
            glAttachShader(program, shaders.compute  );
		glLinkProgram(program);
		GLint linked;
		glGetProgramiv(program,GL_LINK_STATUS, &linked);
		if (!linked) 
		{
			cout << "Program did not link." << endl;
			printLinkInfoLog(program);
		}
	}
}