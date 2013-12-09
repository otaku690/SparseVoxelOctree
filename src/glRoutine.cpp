// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// Camera control code originated from basecode offCIS565 project 6
// copyright (c) 2013 Cheng-Tso Lin  

#define GLM_SWIZZLE
#include <iostream>
#include <vector>
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
shader::ShaderProgram renderVoxelShader;  //Scene-displaying shader program
shader::ShaderProgram voxelizeShader; //Scene-voxelization shader program
shader::ShaderProgram voxelTo3DTexShader; //shader program that converts voxel fragment list to 3D texture

shader::ComputeShader nodeFlagShader;
shader::ComputeShader nodeAllocShader;
shader::ComputeShader nodeInitShader;

shader::ComputeShader octreeTo3DtexShader;

//OpenGL buffer objects
GLuint vbo[10] = {0}; 
GLuint nbo[10] = {0};
GLuint ibo[10] = {0};
GLuint vao[10] = {0};

//voxel dimension
int voxelDim = 8;
int octreeLevel =3;
unsigned int numVoxelFrag = 0;

//voxel-creation rlated buffers
GLuint voxelTex = 0;   //3D texture
GLuint voxelPosTex = 0;  //texture for voxel fragment list (position)
GLuint voxelPosTbo = 0;  //texture buffer object for voxel fragment list (position)

GLuint atomicBuffer = 0;

//Octree pool buffer
GLuint octreeNodeTex = 0;
GLuint octreeNodeTbo = 0;

void glut_display()
{
    renderVoxel();
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
            buildVoxelList();
            break;
        case '2':
            voxelDim = 128;
            createPointCube(voxelDim);
            buildVoxelList();
            break;
        case '3':
            voxelDim = 64;
            createPointCube(voxelDim);
            buildVoxelList();
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

void renderVoxel()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    
    modelview = cam.get_view();
    normalMat = transpose( inverse( mat3( modelview ) ) );

    renderVoxelShader.use();
    renderVoxelShader.setParameter( shader::mat4x4, (void*)&modelview[0][0], "u_ModelView" );
    renderVoxelShader.setParameter( shader::mat4x4, (void*)&projection[0][0], "u_Proj" );
    renderVoxelShader.setParameter( shader::mat3x3, (void*)&normalMat[0][0], "u_Normal" );
    float halfDim = 0.5f/voxelDim;
    renderVoxelShader.setParameter( shader::f1, (void*)&(halfDim), "u_halfDim" );

    glActiveTexture(GL_TEXTURE0);
    glEnable( GL_TEXTURE_3D );
    glBindTexture(GL_TEXTURE_3D, voxelTex );
    renderVoxelShader.setTexParameter( 0, "u_voxel" );


    int numModel = g_meshloader.getModelCount();

    glBindBuffer( GL_ARRAY_BUFFER, vbo[9] );
    glBindVertexArray( vao[0] );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL );
    glEnableVertexAttribArray(0);

    glDrawArrays( GL_POINTS, 0, voxelDim * voxelDim * voxelDim );
    glutSwapBuffers();

    glBindTexture(GL_TEXTURE_3D, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    renderVoxelShader.unuse();
}

void renderScene()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    modelview = cam.get_view();
    normalMat = transpose( inverse( mat3( modelview ) ) );
}

void initShader()
{
    renderVoxelShader.init( "shader/renderVoxel.vert.glsl", "shader/renderVoxel.frag.glsl", "shader/renderVoxel.geom.glsl");

    voxelizeShader.init( "shader/voxelize.vert.glsl", "shader/voxelize.frag.glsl", "shader/voxelize.geom.glsl" );

    octreeTo3DtexShader.init( "shader/fill3Dtex.com.glsl" );

    nodeFlagShader.init( "shader/nodeFlag.com.glsl" );
    nodeAllocShader.init( "shader/nodeAlloc.com.glsl" );
    nodeInitShader.init( "shader/nodeInit.com.glsl" );


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

    glGenVertexArrays( 1, &vao[0] );
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
    GLenum err = glGetError();
    delete [] data;
    return texId;
}

int genLinearBuffer( int size, GLenum format, GLuint* tex, GLuint* tbo )
{
    GLenum err;
   
    if( (*tbo) > 0 )
        glDeleteBuffers( 1, tbo );  //delete previously created tbo

    glGenBuffers( 1, tbo );
   
    glBindBuffer( GL_TEXTURE_BUFFER, *tbo );
    glBufferData( GL_TEXTURE_BUFFER, size, 0, GL_STATIC_DRAW );
    err = glGetError();
    if( (*tex) > 0 )
        glDeleteTextures( 1, tex ); //delete previously created texture

    glGenTextures( 1, tex );
    glBindTexture( GL_TEXTURE_BUFFER, *tex );
    glTexBuffer( GL_TEXTURE_BUFFER, format,  *tbo );
    glBindBuffer( GL_TEXTURE_BUFFER, 0 );

    err = glGetError();
    if( err > 0 )
        cout<<glewGetErrorString(err)<<endl;
    return err;
}

unsigned int genAtomicBuffer( int num, int idx )
{
    GLuint buffer;
    GLuint initVal = 0;

    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, buffer );
    glBufferData( GL_ATOMIC_COUNTER_BUFFER, sizeof( GLuint ), &initVal, GL_STATIC_DRAW );
    glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0 );

    return buffer;
}

//create a cube
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

void voxelizeScene( int bStore )
{
    GLenum err;

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
    voxelizeShader.setParameter( shader::i1, (void*)&bStore, "u_bStore" );

    glBindBufferBase( GL_ATOMIC_COUNTER_BUFFER, 0, atomicBuffer );
    //Bind image in image location 0
  
    if( bStore == 1 )
    {
        glBindImageTexture( 0, voxelPosTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGB10_A2UI );
        voxelizeShader.setTexParameter( 0, "u_voxelPos" );
    }

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
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        
    }
    
    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glViewport( 0, 0, g_width, g_height );

}

void buildVoxelList()
{
    GLenum err;
    
    //Create atomic counter buffer
    if( atomicBuffer > 0 )
        glDeleteBuffers( 1, &atomicBuffer );
    atomicBuffer = genAtomicBuffer( 1, 0 );
 
    voxelizeScene(0);
    glMemoryBarrier( GL_ATOMIC_COUNTER_BARRIER_BIT );

    err = glGetError();
    //Obtain number of voxel fragments
    glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, atomicBuffer );
    GLuint* count = (GLuint*)glMapBufferRange( GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT );
    err = glGetError();

    numVoxelFrag = count[0];

    //Create buffers for voxel fragment list
    genLinearBuffer( sizeof(GLuint) * numVoxelFrag, GL_R32UI, &voxelPosTex, &voxelPosTbo );
    
    //reset counter
    memset( count, 0, sizeof( GLuint ) );

    glUnmapBuffer( GL_ATOMIC_COUNTER_BUFFER );
    glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0 );

    //Voxelize the scene again, this time store the data in the voxel fragment list
    voxelizeScene(1);
    glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

   
}

void buildSVO()
{ 
    GLenum err;

    GLuint allocCounter;
    vector<unsigned int> allocList; //the vector records the number of nodes in each tree level
    allocList.push_back(1); //root level has one node

    //Calculate the maximum possilbe node number
    int totalNode = 1;
    int nTmp = 1;
    for( int i = 1; i <= octreeLevel; ++i )
    { 
        nTmp *= 8;
        totalNode += nTmp;
    }

    //Create an octree node pool with one-eighth maximum node number
    genLinearBuffer( sizeof(GLuint)*totalNode , GL_R32UI, &octreeNodeTex, &octreeNodeTbo );

    //Create an atomic counter for counting # of allocated node tiles, in each octree level
    allocCounter = genAtomicBuffer( 1, 0 );

    //For each octree level (top to bottom), subdivde nodes in 3 steps
    //1. flag nodes that have child nodes ( one thread for each entries in voexl fragment list )
    //2. allocate buffer space for child nodes ( one thread for each node )
    //3. initialize the content of child nodes ( one thread for each node in the new octree level )
    int nodeOffset = 0;
    int allocOffset = 1;
    int groupDim  = (numVoxelFrag + 63)/64;
    for( int i = 0; i < octreeLevel; ++i )
    {
        //node flag
        nodeFlagShader.use();
        nodeFlagShader.setParameter( shader::i1, (void*)&numVoxelFrag, "u_numVoxelFrag" );
        nodeFlagShader.setParameter( shader::i1, (void*)&i, "u_level" );
        nodeFlagShader.setParameter( shader::i1, (void*)&voxelDim, "u_voxelDim" );
        glBindImageTexture( 0, voxelPosTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGB10_A2UI );
        glBindImageTexture( 1, octreeNodeTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI );
        glDispatchCompute( groupDim, 1, 1 );
        glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

        //node tile allocation
        nodeAllocShader.use();
        int numThread = allocList[i];
        nodeAllocShader.setParameter( shader::i1, (void*)&numThread, "u_num" );
        nodeAllocShader.setParameter( shader::i1, (void*)&nodeOffset, "u_start" );
        nodeAllocShader.setParameter( shader::i1, (void*)&allocOffset, "u_allocStart" );
        glBindImageTexture( 0, octreeNodeTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI );
        glBindBufferBase( GL_ATOMIC_COUNTER_BUFFER, 0, allocCounter );

        int allocGroupDim = (allocList[i]+63)/64;
        glDispatchCompute( allocGroupDim, 1, 1 );
        glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT );
        
        //Get the number of node tiles to allocate in the next level
        GLuint titleAllocated;
        GLuint reset = 0;
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, allocCounter );
        glGetBufferSubData( GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &titleAllocated );
        glBufferSubData( GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &reset ); //reset counter to zero
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0 );

        //node tile initialization
        int nodeAllocated = titleAllocated * 8;
        nodeInitShader.use();
        nodeInitShader.setParameter( shader::i1, (void*)&nodeAllocated, "u_num" );
        nodeInitShader.setParameter( shader::i1, (void*)&allocOffset, "u_allocStart" );
        glBindImageTexture( 0, octreeNodeTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI );

        int initGroupDim = ( nodeAllocated + 63 ) / 64;
        glDispatchCompute( initGroupDim, 1, 1 );
        glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

        //update offsets for next level
        allocList.push_back( nodeAllocated ); //titleAllocated * 8 is the number of threads
                                              //we want to launch in the next level
        nodeOffset += allocList[i]; //nodeOffset is the starting node in the next level
        allocOffset += titleAllocated * 8; //allocOffset is the starting address of remaining free space
        err = glGetError();

    }
}

void octreeTo3Dtex()
{
    GLenum err;

    //create 3D texture
    if( voxelTex > 0 ) 
        glDeleteTextures( 1, &voxelTex );
    voxelTex = gen3DTexture( voxelDim );
  

    octreeTo3DtexShader.use();
    octreeTo3DtexShader.setParameter( shader::i1, (void*)&octreeLevel, "u_octreeLevel" );
    octreeTo3DtexShader.setParameter( shader::i1, (void*)&voxelDim, "u_voxelDim" );
    octreeTo3DtexShader.setParameter( shader::i1, (void*)&numVoxelFrag, "u_numVoxelFrag" );
    glBindImageTexture( 0, voxelTex, 0, GL_TRUE, voxelDim, GL_READ_WRITE, GL_R32F );
    glBindImageTexture( 1, octreeNodeTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI );
    glBindImageTexture( 2, voxelPosTex, 0,  GL_FALSE, 0, GL_READ_ONLY, GL_RGB10_A2UI ); 
    int computeDim = voxelDim / 8;
    glDispatchCompute( computeDim, computeDim, computeDim );
    //int computeDim = ( numVoxelFrag + 63 ) /64;
    //glDispatchCompute( computeDim, 1, 1 );
    glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

}