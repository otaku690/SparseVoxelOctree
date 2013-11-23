// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  
#define GLM_SWIZZLE
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include "shader.h"
#include "glRoutine.h"
#include "objLoader.h"
#include "camera.h"
#include "variables.h"

using namespace std;
using namespace glm;

mat4 modelview;
mat4 projection;
mat3 normalMat;

vec3 eyePos = vec3( 0,0,2 );
vec3 eyeLook = vec3(0,0,-1);
vec3 upDir = vec3(0,1,0);
Camera cam( eyePos, eyeLook, upDir );

float FOV = 60.0f;
float zNear = 0.1f;
float zFar = 100.0f;
float aspect;

//Shader
shader::ShaderProgram lightShader;

//OpenGL buffer objects
GLuint vbo[1]; 
GLuint nbo[1];
GLuint ibo[1];
GLuint vao[1];

void glut_display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glCullFace( GL_BACK );

    modelview = cam.get_view();
    normalMat = transpose( inverse( mat3( modelview ) ) );

    lightShader.use();
    lightShader.setParameter( shader::mat4x4, (void*)&modelview[0][0], "u_ModelView" );
    lightShader.setParameter( shader::mat4x4, (void*)&projection[0][0], "u_Proj" );
    lightShader.setParameter( shader::mat3x3, (void*)&normalMat[0][0], "u_Normal" );

    int numModel = g_meshloader.getModelCount();
    for( int i = 0; i < numModel; ++i )
    {
        const ObjModel* model = g_meshloader.getModel(i);
        glBindBuffer( GL_ARRAY_BUFFER, vbo[i] );
        glBindVertexArray( vao[i] );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL );
        glEnableVertexAttribArray(0);

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo[i] );
        glDrawElements( GL_TRIANGLES, model->numIdx, GL_UNSIGNED_INT, (void*)0 );
    }
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

    projection = glm::perspective( FOV, (float)w/(float)h, zNear, zFar );
}

int mouse_buttons = 0;
int mouse_old_x = 0;
int mouse_old_y = 0;

void glut_mouse( int button, int state, int x, int y )
{
    if (state == GLUT_DOWN) 
    {
        mouse_buttons |= 1<<button; 
    } 
    else if (state == GLUT_UP) 
    {
        mouse_buttons = 0;
    }

    mouse_old_x = x;
    mouse_old_y = y;
}

void glut_motion( int x, int y )
{
    float dx, dy;
    dx = (float)(x - mouse_old_x);
    dy = (float)(y - mouse_old_y);

    if (mouse_buttons & 1<<GLUT_RIGHT_BUTTON) {
        cam.adjust(0,0,dx,0,0,0);;
    }
    else {
        cam.adjust(-dx*0.2f,-dy*0.2f,0.0f,0,0,0);
    }

    mouse_old_x = x;
    mouse_old_y = y;
}

void glut_keyboard( unsigned char key, int x, int y )
{
}

void initShader()
{
    lightShader.init( "shader/light.vert.glsl", "shader/light.frag.glsl" );
}

void initVertexData()
{
    int numModel = g_meshloader.getModelCount();
    for( int i = 0; i < numModel; ++i )
    {
        const ObjModel* model = g_meshloader.getModel(i);
        glGenBuffers( 1, &vbo[i] );
        glBindBuffer( GL_ARRAY_BUFFER, vbo[i] );
        glBufferData( GL_ARRAY_BUFFER, sizeof(float) * 3 * model->numVert, model->vbo, GL_STATIC_DRAW  );

        glGenBuffers( 1, &ibo[i] );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo[i] );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned int ) * model->numIdx, model->ibo, GL_STATIC_DRAW );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ARRAY_BUFFER,0 );
    }

    glGenBuffers( 1, &vao[0] );
}