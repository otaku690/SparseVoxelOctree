// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

#include <gl/glew.h>
#include "shader.h"
#include "glslUtility.h"

using namespace glslUtility;

namespace shader
{

ShaderProgram::ShaderProgram()
{
    program = 0;
    vs = 0;
    fs = 0;
    gs = 0;
}

ShaderProgram::~ShaderProgram()
{
    if( program )
    glDeleteProgram( program );
    if(vs )
    glDeleteShader( vs );
    if( fs )
    glDeleteShader( fs );
    if( gs )
    glDeleteShader( gs );
}

int ShaderProgram::init(const char* vs_source, const char* fs_source, const char* gs_source )
{
    //load shader sources and compile
    shaders_t shaderSet = loadShaders( vs_source, fs_source, gs_source );
    vs = shaderSet.vertex;
    fs = shaderSet.fragment;
    gs = shaderSet.geometry;
   
    //create program
    program = glCreateProgram();

    //attach shader 
    attachAndLinkProgram( program, shaderSet );

    return 0;
}

void ShaderProgram::use()
{
    glUseProgram( program );
}
void ShaderProgram::unuse()
{
    glUseProgram( 0 );
}

void ShaderProgram::setParameter( shaderAttrib type, void* param, char* name )
{
    GLint loc  =  glGetUniformLocation( program, name );
    int val = *((int*)param);
    switch( type )
    {
    case i1:
        glUniform1i( glGetUniformLocation( program, name ), *((int*)param) );
        break;
    case f1:
        glUniform1f( glGetUniformLocation( program, name ), *((float*)param) );
        break;
    case fv3:
        glUniform3fv( glGetUniformLocation( program, name ), 1, (float*)param );
        break;
    case fv4:
        glUniform4fv( glGetUniformLocation( program, name ), 1, (float*)param );
        break;
    case mat4x4:
        glUniformMatrix4fv( glGetUniformLocation( program, name ), 1, GL_FALSE, (float*)param );
        break;
    case mat3x3:
        glUniformMatrix3fv( glGetUniformLocation( program, name ), 1, GL_FALSE, (float*)param );
        break;
    case img:
        glUniform1i( glGetUniformLocation( program, name ), *((int*)param) );
        break;
    }
}

void ShaderProgram::setTexParameter( int idx, char* name )
{
    int loc = glGetUniformLocation( program, name );
    glUniform1i( loc, idx );
}

void ShaderProgram::bindAttribLocation( unsigned int idx, char* name )
{
    glBindAttribLocation( program, idx, name );
}

void ShaderProgram::bindFragDataLocation( unsigned int idx, char* name )
{
    glBindFragDataLocation( program, idx, name );
}

ComputeShader::ComputeShader()
{  
}

ComputeShader::~ComputeShader()
{
}

int ComputeShader::init( const char* cs_source )
{
    //load shader sources and compile
    shaders_t shaderSet = loadShaders( NULL, NULL, NULL, cs_source );
   
    //create program
    program = glCreateProgram();

    //attach shader 
    attachAndLinkProgram( program, shaderSet );

    return 0;
}

}