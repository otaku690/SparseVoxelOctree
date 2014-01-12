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
#include "util.h"

using namespace std;
using namespace glm;

const int NUM_RENDERTARGET = 3;

mat4 modelview;
mat4 model;
mat4 view;
mat4 projection;
mat4 normalMat;

vec3 eyePos = vec3(0,0,0 );
vec3 eyeLook = vec3(-1,0,-1);
vec3 upDir = vec3(0,1,0);
Camera cam( eyePos, eyeLook, upDir );

float FOV = 60.0f;
float zNear = 0.05f;
float zFar = 10.0f;
float aspect;

//lighting
Light light1;

//Use this matrix to shift the shadow map coordinate
glm::mat4 biasMatrix(
0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 0.5, 0.0,
0.5, 0.5, 0.5, 1.0
);


//Shader programs-rendering
shader::ShaderProgram renderVoxelShader;  //Scene-displaying shader program

//Shader programs-SVO construction
shader::ShaderProgram voxelizeShader; //Scene-voxelization shader program
shader::ComputeShader nodeFlagShader; 
shader::ComputeShader nodeAllocShader;
shader::ComputeShader nodeInitShader;
shader::ComputeShader leafStoreShader;
shader::ComputeShader mipmapNodeShader;

shader::ComputeShader octreeTo3DtexShader;


//Shader programs-deferred shading
shader::ShaderProgram passShader;
shader::ShaderProgram deferredShader;

//Shader programs - Shadow map 
shader::ShaderProgram shadowmapShader;

//OpenGL buffer objects for loaded mesh
GLuint vbo[10] = {0}; 
GLuint nbo[10] = {0};
GLuint tbo[10] = {0};
GLuint ibo[10] = {0};
GLuint vao[10] = {0};

const int QUAD = 8;
const int VOXEL3DTEX = 9;


//voxel dimension
int voxelDim = 128;
int octreeLevel = 7;
unsigned int numVoxelFrag = 0;

//voxel-creation rlated buffers
GLuint voxelTex = 0;   //3D texture
GLuint voxelPosTex = 0;  //texture for voxel fragment list (position)
GLuint voxelPosTbo = 0;  //texture buffer object for voxel fragment list (position)
GLuint voxelKdTex = 0;  //texture for voxel fragment list (diffuse)
GLuint voxelKdTbo = 0;  //texture buffer object for voxel fragment list (diffuse)
GLuint voxelNrmlTex = 0;  //texture for voxel fragment list (normal)
GLuint voxelNrmlTbo = 0;  //texture buffer object for voxel fragment list (normal)

GLuint atomicBuffer = 0;

//Octree pool buffer
GLuint octreeNodeTex[4] = {0}; //0: child node index
GLuint octreeNodeTbo[4] = {0};

//Textures for deferred shading
GLuint depthFBTex = 0;
GLuint normalFBTex = 0;
GLuint positionFBTex = 0;
GLuint colorFBTex = 0;
GLuint postFBTex = 0;

//Textures for shadowmaping
GLuint shadowmapTex = 0;

//Framebuffer objects
GLuint FBO[2] = {0,0}; //

enum RenderMode render_mode = RENDERSCENE; 
enum Display display_type = DISPLAY_NORMAL; 

void glut_display()
{
    //if( render_mode == RENDERVOXEL )
        
    //else if( render_mode ==  RENDERSCENE )
    

    if( render_mode == RENDERSCENEVOXEL )
    {//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        renderScene(); 
        //renderVoxel();
    }
    else if( render_mode == RENDERSCENE )
    {
        renderScene();
    }
    else if( render_mode == RENDERVOXEL )
    {   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        renderVoxel();
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
    glViewport( 0, 0, w, h );
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    if (FBO[0] != 0 || depthFBTex != 0 || normalFBTex != 0 ) {
        freeFBO();
    }
  
    initFBO(w,h);

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
            glutSetWindowTitle("SVO" );
            render_mode = RENDERSCENE;
            break;
        case '2':
            glutSetWindowTitle("SVO - 256x256x256 voxels" );
            render_mode = RENDERVOXEL;
            if( voxelDim == 256 )
                break;
            voxelDim = 256;
            octreeLevel = 8;
            buildVoxelList();
            buildSVO();
            deleteVoxelList();
            break;
        case '3':
            glutSetWindowTitle("SVO - 128x128x128 voxels" );
            render_mode = RENDERVOXEL;
            if( voxelDim == 128 )
                break;
            voxelDim = 128;
            octreeLevel = 7;
            buildVoxelList();
            buildSVO();
            deleteVoxelList();
            break;
        case ('w'):
            tz = -0.01;
            break;
        case ('s'):
            tz = 0.01;
            break;
        case ('d'):
            tx = -0.01;
            break;
        case ('a'):
            tx = 0.01;
            break;
        case ('q'):
            ty = 0.01;
            break;
        case ('z'):
            ty = -0.01;
            break;
        case 'r':
            initShader();
            break;
   
    }

    if (abs(tx) > 0 ||  abs(tz) > 0 || abs(ty) > 0) {
        cam.adjust(0,0,0,tx,ty,tz);
       
    }
}

void renderVoxel()
{
    //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    
    modelview = cam.get_view();
    normalMat = transpose( inverse( modelview  ) );

    renderVoxelShader.use();
    renderVoxelShader.setParameter( shader::mat4x4, (void*)&modelview[0][0], "u_ModelView" );
    renderVoxelShader.setParameter( shader::mat4x4, (void*)&projection[0][0], "u_Proj" );
    renderVoxelShader.setParameter( shader::mat3x3, (void*)&normalMat[0][0], "u_Normal" );
    renderVoxelShader.setParameter( shader::i1, (void*)&voxelDim, "u_voxelDim" );
    float halfDim = 1.0f/voxelDim;
    renderVoxelShader.setParameter( shader::f1, (void*)&(halfDim), "u_halfDim" );
    renderVoxelShader.setParameter( shader::i1, (void*)&octreeLevel, "u_octreeLevel" );

    glBindImageTexture( 0, octreeNodeTex[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI );
    glBindImageTexture( 1, octreeNodeTex[1], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI );

    //glActiveTexture(GL_TEXTURE0);
    //glEnable( GL_TEXTURE_BUFFER );
    //glBindTexture(GL_TEXTURE_BUFFER, octreeNodeTex[0] );
    //renderVoxelShader.setTexParameter( 0, "u_octreeIdx" );
    //glActiveTexture(GL_TEXTURE1);
    //glEnable( GL_TEXTURE_BUFFER );
    //glBindTexture(GL_TEXTURE_BUFFER, octreeNodeTex[1] );
    //renderVoxelShader.setTexParameter( 1, "u_octreeKd" );

    int numModel = g_meshloader.getModelCount();
    //glBindBuffer( GL_ARRAY_BUFFER, vbo[VOXEL3DTEX] );
    glBindVertexArray( vao[VOXEL3DTEX] );
    //glVertexAttribPointer( 0, 4, GL_UNSIGNED_INT_2_10_10_10_REV, GL_TRUE, 0, (GLubyte*)NULL );
    //glEnableVertexAttribArray(0);

    glDrawArrays( GL_POINTS, 0, voxelDim * voxelDim * voxelDim );
    

    glBindTexture(GL_TEXTURE_3D, 0 );
    //glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
    renderVoxelShader.unuse();
}

void renderScene()
{
    bindFBO(0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable(GL_TEXTURE_2D);

    //PASS 1: render scene attributes to textures
    view = cam.get_view();
    model = mat4(1.0);
    modelview = cam.get_view();
    normalMat = transpose( inverse( modelview ) );

    vec4 lightPos = modelview * light1.pos;
    passShader.use();
    passShader.setParameter( shader::f1, (void*)&zFar, "u_Far" );
    passShader.setParameter( shader::f1, (void*)&zNear, "u_Near" );
    passShader.setParameter( shader::mat4x4, (void*)&model[0][0], "u_Model" );
    passShader.setParameter( shader::mat4x4, (void*)&view[0][0], "u_View" );
    passShader.setParameter( shader::mat4x4, (void*)&projection[0][0], "u_Persp" );
    passShader.setParameter( shader::mat4x4, (void*)&normalMat[0][0], "u_InvTrans" );

    int bTextured;
    int numModel = g_meshloader.getModelCount();
    for( int i = 0; i < numModel; ++i )
    {
        glBindVertexArray( vao[i] );
        const ObjModel* model = g_meshloader.getModel(i);

         glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo[i] );
        for( int i = 0; i < model->numGroup; ++i )
        {
            model->groups[i].shininess = 50;
            passShader.setParameter( shader::fv3, &model->groups[i].kd, "u_Color" );
            passShader.setParameter( shader::f1, &model->groups[i].shininess, "u_shininess" );
            if( model->groups[i].texId > 0 )
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, model->groups[i].texId );
                passShader.setTexParameter( 0, "u_colorTex" );
                bTextured = 1;
            }
            else
                bTextured = 0;
            passShader.setParameter( shader::i1, &bTextured, "u_bTextured" );

            if( model->groups[i].bumpTexId > 0 )
            {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, model->groups[i].bumpTexId );
                passShader.setTexParameter( 1, "u_bumpTex" );
                bTextured = 1;
            }
            else
                bTextured = 0;
            passShader.setParameter( shader::i1, &bTextured, "u_bBump" );

            glDrawElements( GL_TRIANGLES, 3*model->groups[i].numTri , GL_UNSIGNED_INT, (void*)model->groups[i].ibo_offset );
            
        }
    }
      
    //if( render_mode == RENDERSCENEVOXEL )
    //    renderVoxel();

   
    //PASS 2: shadow map generation
    renderShadowMap( light1 );

    //PASS 3: shading
    deferredShader.use();

    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer( GL_FRAMEBUFFER, 0);
   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    mat4 persp = perspective(45.0f,(float)g_width/(float)g_height,zNear,zFar);
    vec4 test(-2,0,10,1);
    vec4 testp = persp * test;
    vec4 testh = testp / testp.w;
    vec2 coords = vec2(testh.x, testh.y) / 2.0f + 0.5f;

    mat4 biasLightMVP = biasMatrix * light1.mvp;
    deferredShader.setParameter( shader::mat4x4, &biasLightMVP[0][0], "u_lightMVP" );
    deferredShader.setParameter( shader::mat4x4, &projection[0][0], "u_persp" );
    deferredShader.setParameter( shader::mat4x4, &modelview[0][0], "u_modelview" );

    deferredShader.setParameter( shader::i1, &g_height, "u_ScreenHeight" );
    deferredShader.setParameter( shader::i1, &g_width, "u_ScreenWidth" );
    deferredShader.setParameter( shader::f1, &zFar, "u_Far" );
    deferredShader.setParameter( shader::f1, &zNear, "u_Near" );
    deferredShader.setParameter( shader::mat4x4, &persp[0][0], "u_Persp" );
    deferredShader.setParameter( shader::i1, &display_type, "u_DisplayType" );

    eyePos = cam.get_pos();
    deferredShader.setParameter( shader::fv4, &lightPos[0], "u_Light" );
    deferredShader.setParameter( shader::fv3, &light1.color[0], "u_LightColor" );
    deferredShader.setParameter( shader::fv3, &eyePos[0], "u_eyePos" );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthFBTex);
    deferredShader.setTexParameter( 0, "u_Depthtex" );

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalFBTex);
    deferredShader.setTexParameter( 1, "u_Normaltex" );

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, positionFBTex);
    deferredShader.setTexParameter( 2, "u_Positiontex" );

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, colorFBTex);
    deferredShader.setTexParameter( 3, "u_Colortex" );

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, shadowmapTex);
    deferredShader.setTexParameter( 4, "u_shadowmap" );

    //Draw the screen space quad
    glBindVertexArray( vao[QUAD] );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[QUAD] );

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,0);

    glBindVertexArray(0);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    glEnable(GL_DEPTH_TEST);

    
}

void renderShadowMap( Light &light )
{
    glBindFramebuffer( GL_FRAMEBUFFER, FBO[1] );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glDisable( GL_CULL_FACE );
    glPolygonOffset(1.1, 4.0);
    shadowmapShader.use();

    mat4 depthProj = glm::perspective<float>(60, 1, zNear, zFar );
    mat4 depthView = glm::lookAt(  vec3(light.pos), vec3(0,0,0), vec3( 1,0,0) );
    mat4 depthModel = mat4(1.0);
    light.mvp = depthProj * depthView * depthModel;

    shadowmapShader.setParameter( shader::mat4x4, &light.mvp[0][0], "u_mvp" );

    int bTextured;
    int numModel = g_meshloader.getModelCount();
    for( int i = 0; i < numModel; ++i )
    {
        glBindVertexArray( vao[i] );
        const ObjModel* model = g_meshloader.getModel(i);

         glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo[i] );
        for( int i = 0; i < model->numGroup; ++i )
        {
            model->groups[i].shininess = 50;
            shadowmapShader.setParameter( shader::fv3, &model->groups[i].kd, "u_Color" );
            shadowmapShader.setParameter( shader::f1, &model->groups[i].shininess, "u_shininess" );
            if( model->groups[i].texId > 0 )
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, model->groups[i].texId );
                shadowmapShader.setTexParameter( 0, "u_colorTex" );
                bTextured = 1;
            }
            else
                bTextured = 0;
            shadowmapShader.setParameter( shader::i1, &bTextured, "u_bTextured" );

            if( model->groups[i].bumpTexId > 0 )
            {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, model->groups[i].bumpTexId );
                shadowmapShader.setTexParameter( 1, "u_bumpTex" );
                bTextured = 1;
            }
            else
                bTextured = 0;
            shadowmapShader.setParameter( shader::i1, &bTextured, "u_bBump" );

            glDrawElements( GL_TRIANGLES, 3*model->groups[i].numTri , GL_UNSIGNED_INT, (void*)model->groups[i].ibo_offset );
            
        }
    }
    glPolygonOffset(0,0);
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void initFBO( int w, int h )
{
    GLenum FBOstatus;
    GLenum err;

    glActiveTexture(GL_TEXTURE9);

    glGenTextures(1, &depthFBTex);
    glGenTextures(1, &normalFBTex);
    glGenTextures(1, &positionFBTex);
    glGenTextures(1, &colorFBTex);

    //Set up depth FBO
    depthFBTex = gen2DTexture( w, h, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT );

    //Set up normal FBO
    normalFBTex = gen2DTexture( w, h, GL_RGBA32F, GL_RGBA, GL_FLOAT );

    //Set up position FBO
    positionFBTex = gen2DTexture( w, h, GL_RGBA32F, GL_RGBA, GL_FLOAT );

    //Set up color FBO
    colorFBTex = gen2DTexture( w, h, GL_RGBA32F, GL_RGBA, GL_FLOAT );

    // create a framebuffer object
    glGenFramebuffers(1, &FBO[0]);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO[0]);

    // Instruct openGL that we won't bind a color texture with the currently bound FBO
    glReadBuffer(GL_NONE);

    GLenum draws [NUM_RENDERTARGET];
    draws[0] = GL_COLOR_ATTACHMENT0;
    draws[1] = GL_COLOR_ATTACHMENT1;
    draws[2] = GL_COLOR_ATTACHMENT2;

    glDrawBuffers(NUM_RENDERTARGET, draws);

    // attach the texture to FBO depth attachment point
    glBindTexture(GL_TEXTURE_2D, depthFBTex);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthFBTex, 0);

    glBindTexture(GL_TEXTURE_2D, normalFBTex);    
    glFramebufferTexture(GL_FRAMEBUFFER, draws[0], normalFBTex, 0);

    glBindTexture(GL_TEXTURE_2D, positionFBTex);    
    glFramebufferTexture(GL_FRAMEBUFFER, draws[1], positionFBTex, 0);

    glBindTexture(GL_TEXTURE_2D, colorFBTex);    
    glFramebufferTexture(GL_FRAMEBUFFER, draws[2], colorFBTex, 0);

    
    // check FBO status
    FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(FBOstatus != GL_FRAMEBUFFER_COMPLETE) {
        printf("GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO[0]\n");
        checkFramebufferStatus(FBOstatus);
    }

   
    //Shadow map buffers
    glGenTextures(1, &shadowmapTex );
  
    //Set up shadow map FB texture
    glBindTexture(GL_TEXTURE_2D, shadowmapTex );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32 , 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0 );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    

    // create a framebuffer object for shadow mapping
    glGenFramebuffers(1, &FBO[1]);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO[1]);

    glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowmapTex, 0 );
    glDrawBuffer( GL_NONE ); //Disable render


    // check FBO status
    FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(FBOstatus != GL_FRAMEBUFFER_COMPLETE) {
        printf("GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO[1]\n");
        checkFramebufferStatus(FBOstatus);
    }
    err = glGetError();

    // switch back to window-system-provided framebuffer
    glClear( GL_DEPTH_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void freeFBO() 
{
    glDeleteTextures(1,&depthFBTex);
    glDeleteTextures(1,&normalFBTex);
    glDeleteTextures(1,&positionFBTex);
    glDeleteTextures(1,&colorFBTex);
    //glDeleteTextures(1,&postFBTex);
    glDeleteFramebuffers(1,&FBO[0]);
    glDeleteFramebuffers(1,&FBO[1]);
}

void bindFBO(int buf)
{
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0); //Bad mojo to unbind the framebuffer using the texture
    glBindFramebuffer(GL_FRAMEBUFFER, FBO[buf]);
    glClear(GL_DEPTH_BUFFER_BIT);
    //glColorMask(false,false,false,false);
    glEnable(GL_DEPTH_TEST);
}

void setTextures() 
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //glColorMask(true,true,true,true);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
}

void initShader()
{
    renderVoxelShader.init( "shader/renderVoxel.vert.glsl", "shader/renderVoxel.frag.glsl", "shader/renderVoxel.geom.glsl");

    voxelizeShader.init( "shader/voxelize.vert.glsl", "shader/voxelize.frag.glsl", "shader/voxelize.geom.glsl" );

    octreeTo3DtexShader.init( "shader/fill3Dtex.com.glsl" );

    nodeFlagShader.init( "shader/nodeFlag.com.glsl" );
    nodeAllocShader.init( "shader/nodeAlloc.com.glsl" );
    nodeInitShader.init( "shader/nodeInit.com.glsl" );

    leafStoreShader.init( "shader/leafStore.com.glsl" );
    //mipmapNodeShader.init( "shader/mipmapNode.com.glsl" );

    passShader.init( "shader/pass.vert.glsl", "shader/pass.frag.glsl" );
    deferredShader.init( "shader/shade.vert.glsl", "shader/diagnostic.frag.glsl" );

    shadowmapShader.init( "shader/shadowmap.vert.glsl", "shader/shadowmap.frag.glsl" );
}

void initVertexData()
{
    //Send loaded models to GPU
    int numModel = g_meshloader.getModelCount();
    for( int i = 0; i < numModel; ++i )
    {
        glGenVertexArrays( 1, &vao[i] );
        glBindVertexArray( vao[i] );
        const ObjModel* model = g_meshloader.getModel(i);

        glGenBuffers( 1, &vbo[i] );
        glBindBuffer( GL_ARRAY_BUFFER, vbo[i] );
        glBufferData( GL_ARRAY_BUFFER, sizeof(float) * 3 * model->numVert, model->vbo, GL_STATIC_DRAW  );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
        glEnableVertexAttribArray( 0 );

        if( model->numNrml > 0 )
        {
            glGenBuffers( 1, &nbo[i] );
            glBindBuffer( GL_ARRAY_BUFFER, nbo[i] );
            glBufferData( GL_ARRAY_BUFFER, sizeof(float) * 3 * model->numVert, model->nbo, GL_STATIC_DRAW  );
            glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );
            glEnableVertexAttribArray( 1 );
        }
        if( model->numTxcoord > 0 )
        {
            glGenBuffers( 1, &tbo[i] );
            glBindBuffer( GL_ARRAY_BUFFER, tbo[i] );
            glBufferData( GL_ARRAY_BUFFER, sizeof(float) * 2 * model->numVert, model->tbo, GL_STATIC_DRAW  );
            glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, 0 );
            glEnableVertexAttribArray( 2 );
        }

        glGenBuffers( 1, &ibo[i] );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo[i] );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned int ) * model->numIdx, model->ibo, GL_STATIC_DRAW );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ARRAY_BUFFER,0 );
        glBindVertexArray(0);

        //Upload texture to GPU 
        for( int i = 0; i < model->numGroup; ++i )
        {
            if( model->groups[i].tex_filename.length() > 0 )
                model->groups[i].texId = loadTexturFromFile( model->groups[i].tex_filename.c_str(), GL_RGB8, GL_BGR, 2 );
            else
                model->groups[i].texId = 0;
            if( model->groups[i].bump_filename.length() > 0 )
                model->groups[i].bumpTexId = loadTexturFromFile( model->groups[i].bump_filename.c_str(), GL_RGB8, GL_BGR, 0 );
            else
                model->groups[i].bumpTexId = 0;
        }
    }
    

    //Create a cube comprised of points, for voxel visualization
    createPointCube( voxelDim );

    //create a screen-size quad for deferred shading
    createScreenQuad();

    
}

unsigned int gen2DTexture( int w, int h, GLenum internalFormat,  GLenum format, GLenum type )
{
    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture( GL_TEXTURE_2D, texId );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

    glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, 0);

    glBindTexture( GL_TEXTURE_2D, 0 );
    return texId;
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
    glTexImage3D( GL_TEXTURE_3D, 0, GL_R8, dim, dim, dim, 0, GL_RED, GL_UNSIGNED_BYTE, data );
    glBindTexture( GL_TEXTURE_3D, 0 );
    GLenum err = glGetError();
    cout<<glewGetErrorString(err)<<" "<<err<<endl;
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

void genAtomicBuffer( int num, unsigned int &buffer )
{
    GLuint initVal = 0;

    if( buffer )
        glDeleteBuffers( 1, &buffer );
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, buffer );
    glBufferData( GL_ATOMIC_COUNTER_BUFFER, sizeof( GLuint ), &initVal, GL_STATIC_DRAW );
    glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0 );

}

//create a screen-size quad
void createScreenQuad()
{
    GLenum err;
    vertex2_t verts [] = { {vec3(-1,1,0),vec2(0,1)},
        {vec3(-1,-1,0),vec2(0,0)},
        {vec3(1,-1,0),vec2(1,0)},
        {vec3(1,1,0),vec2(1,1)}};

    unsigned short indices[] = { 0,1,2,0,2,3};

    glGenVertexArrays( 1, &vao[QUAD] );
    glBindVertexArray( vao[QUAD] );

    //Allocate vbos for data
    glGenBuffers(1,&vbo[QUAD]);
    glGenBuffers(1,&ibo[QUAD]);

    //Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[QUAD]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    //Use of strided data, Array of Structures instead of Structures of Arrays
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(vertex2_t),0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,sizeof(vertex2_t),(void*)sizeof(vec3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    //indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[QUAD]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLushort), indices, GL_STATIC_DRAW);

    //Unplug Vertex Array
    glBindVertexArray(0);

    err = glGetError();
}

//create a cube
// Don't need this anymore, instead we use gl_vertexID in vertex shader to index the 3D texture
void createPointCube( int dim )
{
    GLenum err;
    //GLfloat* data = new GLfloat[ 3 * dim * dim * dim ];
    //GLuint* data = new GLuint[dim*dim*dim];
    //memset( data, 0, sizeof(GLuint) * dim * dim * dim );
    //int yoffset, offset;
    //for( int y = 0; y < dim; ++y )
    //{
    //    yoffset = y*dim*dim;
    //    for( int z = 0; z < dim; ++z )
    //    {
    //        offset = yoffset + z*dim;
    //        for( int x = 0; x < dim; ++x )
    //        {
    //            //data[ 3*( offset + x ) ] = x /(float)voxelDim;
    //            //data[ 3*( offset + x )+1 ] = y /(float)voxelDim ;
    //            //data[ 3*( offset + x )+2 ] = z /(float)voxelDim ;

    //            //Pack point cloud position in GL_UNSIGNED_INT_2_10_10_10_REV format
    //            data[offset+x ] |= (1<<30);
    //            data[ offset+x ] |= (x<<20);
    //            data[ offset+x ] |= (y<<10);
    //            data[ offset+x ] |= (z);
    //        }
    //    }
    //}

    //if( vbo[VOXEL3DTEX] ) 
    //    glDeleteBuffers(1, &vbo[VOXEL3DTEX] );
    //glGenBuffers( 1, &vbo[VOXEL3DTEX] );
    //glBindBuffer( GL_ARRAY_BUFFER, vbo[VOXEL3DTEX] );
    //glBufferData( GL_ARRAY_BUFFER, sizeof(GLuint) *  voxelDim * voxelDim * voxelDim, data, GL_STATIC_DRAW );
    

    if( vao[VOXEL3DTEX] )
        glDeleteVertexArrays( 1, &vao[VOXEL3DTEX] );
    glGenVertexArrays( 1, &vao[VOXEL3DTEX] );
    glBindVertexArray( vao[VOXEL3DTEX] );
    //glVertexAttribPointer( 0, 4 , GL_UNSIGNED_INT_2_10_10_10_REV , GL_FALSE, 0, 0 );
    //glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    //delete [] data;
  
    err = glGetError();
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
        glBindImageTexture( 1, voxelKdTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8 );
        glBindImageTexture( 2, voxelNrmlTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F );
        voxelizeShader.setTexParameter( 0, "u_voxelPos" );
    }

    int bTextured;
    int numModel = g_meshloader.getModelCount();
    for( int i = 0; i < numModel; ++i )
    {
        glBindVertexArray( vao[i] );
        const ObjModel* model = g_meshloader.getModel(i);

         glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo[i] );
        for( int i = 0; i < model->numGroup; ++i )
        {
            model->groups[i].shininess = 50;
            voxelizeShader.setParameter( shader::fv3, &model->groups[i].kd, "u_Color" );
            voxelizeShader.setParameter( shader::f1, &model->groups[i].shininess, "u_shininess" );
            if( model->groups[i].texId > 0 )
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, model->groups[i].texId );
                voxelizeShader.setTexParameter( 0, "u_colorTex" );
                bTextured = 1;
            }
            else
                bTextured = 0;
            voxelizeShader.setParameter( shader::i1, &bTextured, "u_bTextured" );

            if( model->groups[i].bumpTexId > 0 )
            {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, model->groups[i].bumpTexId );
                voxelizeShader.setTexParameter( 1, "u_bumpTex" );
                bTextured = 1;
            }
            else
                bTextured = 0;
            voxelizeShader.setParameter( shader::i1, &bTextured, "u_bBump" );

            glDrawElements( GL_TRIANGLES, 3*model->groups[i].numTri , GL_UNSIGNED_INT, (void*)model->groups[i].ibo_offset );
            
        }
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

    genAtomicBuffer( 1, atomicBuffer );
 
    voxelizeScene(0);
    glMemoryBarrier( GL_ATOMIC_COUNTER_BARRIER_BIT );

    err = glGetError();
    //Obtain number of voxel fragments
    glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, atomicBuffer );
    GLuint* count = (GLuint*)glMapBufferRange( GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT );
    err = glGetError();

    numVoxelFrag = count[0];
    cout<<"Number of Entries in Voxel Fragment List: "<<numVoxelFrag<<endl;
    //Create buffers for voxel fragment list
    genLinearBuffer( sizeof(GLuint) * numVoxelFrag, GL_R32UI, &voxelPosTex, &voxelPosTbo );
    genLinearBuffer( sizeof(GLuint) * numVoxelFrag, GL_RGBA8, &voxelKdTex, &voxelKdTbo ); 
    //genLinearBuffer( sizeof(GLuint) * numVoxelFrag, GL_RGBA16F, &voxelNrmlTex, &voxelNrmlTbo ); 
    
    //reset counter
    memset( count, 0, sizeof( GLuint ) );

    glUnmapBuffer( GL_ATOMIC_COUNTER_BUFFER );
    glBindBuffer( GL_ATOMIC_COUNTER_BUFFER, 0 );

    //Voxelize the scene again, this time store the data in the voxel fragment list
    voxelizeScene(1);
    glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

   
}

void deleteVoxelList()
{
    glDeleteTextures( 1, &voxelPosTex );
    voxelPosTex = 0;
    glDeleteBuffers( 1, &voxelPosTbo );
    voxelPosTbo = 0;
}

void buildSVO()
{ 
    GLenum err;

    GLuint allocCounter = 0;
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
    cout<<"Max possible node: "<<totalNode<<endl;
    //Heuristically cut down the allocation size
    //totalNode = 10000000;
    cout<<"Size of node pool allocated: "<<totalNode<<endl;
    //Create an octree node pool
    //This is for storing child node indices
    genLinearBuffer( sizeof(GLuint)*totalNode , GL_R32UI, &octreeNodeTex[0], &octreeNodeTbo[0] );

    //This is for storing diffuse 
    genLinearBuffer( sizeof(GLuint)*totalNode , GL_R32UI, &octreeNodeTex[1], &octreeNodeTbo[1] );

    //Create an atomic counter for counting # of allocated node tiles, in each octree level
    genAtomicBuffer( 1, allocCounter );
     err = glGetError();
        cout<<"SVO build alloc "<<glewGetErrorString(err)<<" Error code:"<<err<<endl;
    //For each octree level (top to bottom), subdivde nodes in 3 steps
    //1. flag nodes that have child nodes ( one thread for each entries in voexl fragment list )
    //2. allocate buffer space for child nodes ( one thread for each node )
    //3. initialize the content of child nodes ( one thread for each node in the new octree level )
    int nodeOffset = 0;
    int allocOffset = 1;
    int dataWidth = 1024;
    int dataHeight = (numVoxelFrag+1023) / dataWidth;
    int groupDimX = dataWidth/8;
    int groupDimY = (dataHeight+7)/8;
    for( int i = 0; i < octreeLevel; ++i )
    {
        //node flag
        nodeFlagShader.use();
        nodeFlagShader.setParameter( shader::i1, (void*)&numVoxelFrag, "u_numVoxelFrag" );
        nodeFlagShader.setParameter( shader::i1, (void*)&i, "u_level" );
        nodeFlagShader.setParameter( shader::i1, (void*)&voxelDim, "u_voxelDim" );
        glBindImageTexture( 0, voxelPosTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGB10_A2UI );
        glBindImageTexture( 1, octreeNodeTex[0], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI );
        glDispatchCompute(groupDimX, groupDimY, 1 );
        glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

        //node tile allocation
        nodeAllocShader.use();
        int numThread = allocList[i];
        nodeAllocShader.setParameter( shader::i1, (void*)&numThread, "u_num" );
        nodeAllocShader.setParameter( shader::i1, (void*)&nodeOffset, "u_start" );
        nodeAllocShader.setParameter( shader::i1, (void*)&allocOffset, "u_allocStart" );
        glBindImageTexture( 0, octreeNodeTex[0], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI );
        glBindBufferBase( GL_ATOMIC_COUNTER_BUFFER, 0, allocCounter );

        int allocGroupDim = (allocList[i]+63)/64;
        glDispatchCompute( allocGroupDim, 1, 1 );
        glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT );
        
        //Get the number of node tiles to allocate in the next level
        GLuint tileAllocated;
        GLuint reset = 0;
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, allocCounter );
        glGetBufferSubData( GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &tileAllocated );
        glBufferSubData( GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &reset ); //reset counter to zero
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0 );

        //node tile initialization
        int nodeAllocated = tileAllocated * 8;
        nodeInitShader.use();
        nodeInitShader.setParameter( shader::i1, (void*)&nodeAllocated, "u_num" );
        nodeInitShader.setParameter( shader::i1, (void*)&allocOffset, "u_allocStart" );
        glBindImageTexture( 0, octreeNodeTex[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI );
        glBindImageTexture( 1, octreeNodeTex[1], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI );

        dataWidth = 1024;
        dataHeight = (nodeAllocated +1023) / dataWidth;
        int initGroupDimX =dataWidth/8;
        int initGroupDimY = (dataHeight+7)/8;
        glDispatchCompute( initGroupDimX, initGroupDimY, 1 );
        glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

        //update offsets for next level
        allocList.push_back( nodeAllocated ); //titleAllocated * 8 is the number of threads
                                              //we want to launch in the next level
        nodeOffset += allocList[i]; //nodeOffset is the starting node in the next level
        allocOffset += nodeAllocated; //allocOffset is the starting address of remaining free space
       
        err = glGetError();
        cout<<"SVO build iteration "<<glewGetErrorString(err)<<" Error code:"<<err<<endl;
    }
    cout<<"Total nodes consumed: "<<allocOffset<<endl;

    //flag nonempty leaf nodes
    nodeFlagShader.use();
    nodeFlagShader.setParameter( shader::i1, (void*)&numVoxelFrag, "u_numVoxelFrag" );
    nodeFlagShader.setParameter( shader::i1, (void*)&octreeLevel, "u_level" );
    nodeFlagShader.setParameter( shader::i1, (void*)&voxelDim, "u_voxelDim" );
    glBindImageTexture( 0, voxelPosTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGB10_A2UI );
    glBindImageTexture( 1, octreeNodeTex[0], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI );
    glDispatchCompute( groupDimX, groupDimY, 1 );
    glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

    //Store surface information ( Color, normal, etc. ) into the octree leaf nodes
    leafStoreShader.use();
    leafStoreShader.setParameter( shader::i1, (void*)&numVoxelFrag, "u_numVoxelFrag" );
    leafStoreShader.setParameter( shader::i1, (void*)&octreeLevel, "u_octreeLevel" );
    leafStoreShader.setParameter( shader::i1, (void*)&voxelDim, "u_voxelDim" );
    glBindImageTexture( 0, octreeNodeTex[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI );
    glBindImageTexture( 1, octreeNodeTex[1], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI );
    glBindImageTexture( 2, voxelPosTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGB10_A2UI );
    glBindImageTexture( 3, voxelKdTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8 );
    glDispatchCompute( groupDimX, groupDimY, 1 );
    glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

    glDeleteBuffers( 1, &allocCounter );
    allocCounter = 0;
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

    glBindImageTexture( 0, voxelTex, 0, GL_TRUE, voxelDim, GL_WRITE_ONLY, GL_R8 );

    glBindImageTexture( 1, octreeNodeTex[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI );
    glBindImageTexture( 2, voxelPosTex, 0,  GL_FALSE, 0, GL_READ_ONLY, GL_RGB10_A2UI );
    glBindImageTexture( 3, voxelKdTex, 0,  GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F); 
    glBindImageTexture( 4, voxelNrmlTex, 0,  GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F ); 

    int computeDim = (voxelDim+7) / 8;
    glDispatchCompute( computeDim, computeDim, computeDim );
    //int computeDim = ( numVoxelFrag + 63 ) /64;
    //glDispatchCompute( computeDim, 1, 1 );
    glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

}

void initLight()
{
    light1.pos = light1.initialPos = vec4( 0, 0.43, 0, 1 );
    light1.color = vec3( 1, 1, 1 );
}