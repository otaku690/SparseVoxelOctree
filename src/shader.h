// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

#ifndef _SHADER_H
#define _SHADER_H

namespace shader
{

enum shaderAttrib{
    i1, fv3, fv4, f1, mat4x4, mat3x3, tex, img
};

class ShaderProgram
{
public:
    ShaderProgram();
    virtual ~ShaderProgram();
    int init( const char* vs_source, const char* fs_source, const char* gs_source = 0 );
    void use();
    void unuse();
    void setParameter( shaderAttrib type, void* param, char* name );
    void setTexParameter( int idx, char* name );
protected:
    GLuint vs; //vertex shader
    GLuint fs; //fragment shader
    GLuint gs; //geometry shader
    GLuint program;
};

class ComputeShader: public ShaderProgram
{
public:
    ComputeShader();
    ~ComputeShader();
    int init( const char* cs_source );
};

}
#endif