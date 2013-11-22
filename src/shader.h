#ifndef _SHADER_H
#define _SHADER_H

#include <gl/glew.h>

enum shaderAttrib{
    fv3, fv4, f1, mat4, mat3, tex
};

#endif

class ShaderProgram
{
public:
    ShaderProgram();
    virtual ~ShaderProgram();
    int init( const char* vs_source, const char* fs_source, const char* gs_source = 0 );
    void use();
    void unuse();
    void setParameter( shaderAttrib type, void* param, char* name );
private:
    GLuint vs; //vertex shader
    GLuint fs; //fragment shader
    GLuint gs; //geometry shader
    GLuint program;
};