// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// Camera control code originated from basecode offCIS565 project 6
// copyright (c) 2013 Cheng-Tso Lin  

#define GLM_SWIZZLE
#include <iostream>
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

vec3 eyePos = vec3(0.5,0.5,2 );
vec3 eyeLook = vec3(0,0,-1);
vec3 upDir = vec3(0,1,0);
Camera cam( eyePos, eyeLook, upDir );

float FOV = 60.0f;
float zNear = 0.1f;
float zFar = 100.0f;
float aspect;

//Shader programs
shader::ShaderProgram lightShader;
shader::ShaderProgram voxelizeShader;

//OpenGL buffer objects
GLuint vbo[10] = {0}; 
GLuint nbo[10] = {0};
GLuint ibo[10] = {0};
GLuint vao[10] = {0};

//voxel dimension
int voxelDim = 256;
GLuint voxelTex = 0;

void glut_display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    
    modelview = cam.get_view();
    normalMat = transpose( inverse( mat3( modelview ) ) );
    //glViewport(0,0,512,512 );
    //mat4 Ortho = glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f, zNear, zFar );

    ////Create an modelview-orthographic projection matrix see from X axis
    //mat4 mvpX = Ortho * glm::lookAt( vec3( 5, 0, 0 ), vec3( 0, 0, 0 ), vec3( 0, 1, 0 ) );

    ////Create an modelview-orthographic projection matrix see from X axis
    //mat4 mvpY = Ortho * glm::lookAt( vec3( 0, 5, 0 ), vec3( 0, 0, 0 ), vec3( 0, 0, -1 ) );

    ////Create an modelview-orthographic projection matrix see from X axis
    //mat4 mvpZ = Ortho * glm::lookAt( vec3( 0, 0, 5 ), vec3( 0, 0, 0 ), vec3( 0, 1, 0 ) );

    lightShader.use();
    lightShader.setParameter( shader::mat4x4, (void*)&modelview[0][0], "u_ModelView" );
    lightShader.setParameter( shader::mat4x4, (void*)&projection[0][0], "u_Proj" );
    lightShader.setParameter( shader::mat3x3, (void*)&normalMat[0][0], "u_Normal" );
    float halfDim = 0.5f/voxelDim;
    lightShader.setParameter( shader::f1, (void*)&(halfDim), "u_halfDim" );

    glActiveTexture(GL_TEXTURE0);
    glEnable( GL_TEXTURE_3D );
    glBindTexture(GL_TEXTURE_3D, voxelTex );
    lightShader.setTexParameter( 0, "u_voxel" );
    //lightShader.setParameter( shader::mat4x4, (void*)&mvpX[0][0], "u_MVPx" );
    //lightShader.setParameter( shader::mat4x4, (void*)&mvpY[0][0], "u_MVPy" );
    //lightShader.setParameter( shader::mat4x4, (void*)&mvpZ[0][0], "u_MVPz" );

    int numModel = g_meshloader.getModelCount();
    //for( int i = 0; i < numModel; ++i )
    //{
    //    const ObjModel* model = g_meshloader.getModel(i);
    //    glBindBuffer( GL_ARRAY_BUFFER, vbo[i] );
    //    glBindVertexArray( vao[i] );
    //    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL );
    //    glEnableVertexAttribArray(0);

    //    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo[i] );
    //    glDrawElements( GL_TRIANGLES, model->numIdx, GL_UNSIGNED_INT, (void*)0 );
    //}

    glBindBuffer( GL_ARRAY_BUFFER, vbo[9] );
    glBindVertexArray( vao[0] );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL );
    glEnableVertexAttribArray(0);

    glDrawArrays( GL_POINTS, 0, voxelDim * voxelDim * voxelDim );
    glutSwapBuffers();

    glBindTexture(GL_TEXTURE_3D, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    lightShader.unuse();
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
    //projection = glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f, zNear, zFar );
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
    float tx = 0;
    float ty = 0;
    float tz = 0;
    switch(key) {
        case (27):
            exit(0.0);
            break;
        case '1':
            voxelDim = 256;
            createPointCube(voxelDim);
            voxelizeScene();
            break;
        case '2':
            voxelDim = 128;
            createPointCube(voxelDim);
            voxelizeScene();
            break;
        case '3':
            voxelDim = 64;
            createPointCube(voxelDim);
            voxelizeScene();
            break;
        case ('w'):
            tz = -0.1;
            break;
        case ('s'):
            tz = 0.1;
            break;
        case ('d'):
            tx = -0.1;
            break;
        case ('a'):
            tx = 0.1;
            break;
        case ('q'):
            ty = 0.1;
            break;
        case ('z'):
            ty = -0.1;
            break;
   
    }

    if (abs(tx) > 0 ||  abs(tz) > 0 || abs(ty) > 0) {
        cam.adjust(0,0,0,tx,ty,tz);
    }
}

void initShader()
{
    lightShader.init( "shader/light.vert.glsl", "shader/light.frag.glsl", "shader/light.geom.glsl");
    voxelizeShader.init( "shader/voxelize.vert.glsl", "shader/voxelize.frag.glsl", "shader/voxelize.geom.glsl" );
}

void initVertexData()
{
    //Send loaded models to GPU
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

    //Create a cube comprised of points, for voxel visualization
    createPointCube( voxelDim );

    glGenBuffers( 1, &vao[0] );
}

unsigned int gen3DTexture( int dim )
{
    float* data = new float[dim*dim*dim];
    memset( data, 0, sizeof(float)*dim*dim*dim );

    GLuint texId;
    glGenTextures( 1, &texId );
    glBindTexture( GL_TEXTURE_3D, texId );
    //glActiveTexture(GL_TEXTURE0 );
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_3D,  GL_TEXTURE_MIN_FILTER, GL_NEAREST);  
    glTexParameteri(GL_TEXTURE_3D,  GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    glTexImage3D( GL_TEXTURE_3D, 0, GL_R32F, dim, dim, dim, 0, GL_RED, GL_FLOAT, data );
    glBindTexture( GL_TEXTURE_3D, 0 );

    delete [] data;
    return texId;
}

void createPointCube( int dim )
{
    float* data = new float[ 3 * dim * dim * dim ];
    int yoffset, offset;
    for( int y = 0; y < dim; ++y )
    {
        yoffset = y*dim*dim;
        for( int z = 0; z < dim; ++z )
        {
            offset = yoffset + z*dim;
            for( int x = 0; x < dim; ++x )
            {
                data[ 3*( offset + x ) ] = x / (float)dim;
                data[ 3*( offset + x )+1 ] = y / (float)dim;
                data[ 3*( offset + x )+2 ] = z / (float)dim;
            }
        }
    }

    if( vbo[9] ) 
        glDeleteBuffers(1, &vbo[9] );
    glGenBuffers( 1, &vbo[9] );
    glBindBuffer( GL_ARRAY_BUFFER, vbo[9] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(float) * 3 * voxelDim * voxelDim * voxelDim, data, GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    delete [] data;
  
}

void voxelizeScene()
{
    //create 3D texture
    if( voxelTex > 0 ) glDeleteTextures( 1, &voxelTex );
    voxelTex = gen3DTexture( voxelDim );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glViewport( 0, 0,  voxelDim,  voxelDim );
    //Orthograhic projection
    mat4 Ortho; 
    //Create an modelview-orthographic projection matrix see from +X axis
    Ortho = glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f, 2.0f-1.0f, 3.0f );

    mat4 mvpX = Ortho * glm::lookAt( vec3( 2, 0, 0 ), vec3( 0, 0, 0 ), vec3( 0, 1, 0 ) );

    //Create an modelview-orthographic projection matrix see from +Y axis
    mat4 mvpY = Ortho * glm::lookAt( vec3( 0, 2, 0 ), vec3( 0, 0, 0 ), vec3( 0, 0, -1 ) );

    //Create an modelview-orthographic projection matrix see from +Z axis
    mat4 mvpZ = Ortho * glm::lookAt( vec3( 0, 0, 2 ), vec3( 0, 0, 0 ), vec3( 0, 1, 0 ) );



    //Disable some fixed-function opeartions
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );


    voxelizeShader.use();
    voxelizeShader.setParameter( shader::mat4x4, (void*)&mvpX[0][0], "u_MVPx" );
    voxelizeShader.setParameter( shader::mat4x4, (void*)&mvpY[0][0], "u_MVPy" );
    voxelizeShader.setParameter( shader::mat4x4, (void*)&mvpZ[0][0], "u_MVPz" );
    voxelizeShader.setParameter( shader::i1, (void*)&voxelDim, "u_width" );
    voxelizeShader.setParameter( shader::i1, (void*)&voxelDim, "u_height" );

    
    //glEnable( GL_TEXTURE_3D );
    //glActiveTexture( GL_TEXTURE0 );
    //glBindTexture( GL_TEXTURE_3D, voxelTex );
    glBindImageTexture( 0, voxelTex, 0, GL_TRUE, voxelDim, GL_READ_WRITE, GL_R32F );
    voxelizeShader.setTexParameter( 0, "u_voxelImage" );

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
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }

    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glViewport( 0, 0, g_width, g_height );
    //glutSwapBuffers();
}