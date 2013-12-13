// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

#ifndef _GLROUTINE_H
#define _GLROUTINE_H

typedef struct {
	glm::vec3 pt;
	glm::vec2 texcoord;
} vertex2_t;

enum Display {
    DISPLAY_DEPTH = 0,
    DISPLAY_NORMAL = 1,
    DISPLAY_POSITION = 2,
    DISPLAY_COLOR = 3,
    DISPLAY_TOTAL = 4,
    DISPLAY_LIGHTS = 5,
    DISPLAY_TOON = 6
};

enum RenderMode {
    RENDERVOXEL = 1,
    RENDERSCENE = 2,
    RENDERSCENEVOXEL = 3
};

enum LightType
{ 
    DIRECTIONAL = 1,
    PROJECTIVE = 2,
    POINTED = 3,
};

struct Light
{
    glm::vec4 initialPos;
    glm::vec4 pos;
    glm::vec3 color;

    glm::mat4 mvp; //each light has its own projection matrix
};



//////////////// GLUT callback functions
void glut_display();
void glut_reshape( int w, int h );
void glut_idle();
void glut_mouse( int button, int state, int x, int y );
void glut_motion( int x, int y );
void glut_keyboard( unsigned char key, int x, int y );
////////////////

///rendering routines
void renderVoxel();
void renderScene();
void renderShadowMap( Light &light );

///Framebuffer objects initialization
void initFBO( int w, int h );
void freeFBO();

void bindFBO(int buf);
void setTextures();

///Scene voxelization
void voxelizeScene(int bStore);
void buildVoxelList();
void buildSVO();

void deleteVoxelList();
////

void initShader();
void initVertexData();
void createPointCube( int dim );
void createScreenQuad();

///2D texture generation
unsigned int gen2DTexture( int w, int h, GLenum internalFormat,  GLenum format, GLenum type );

///3D texture generation
unsigned int gen3DTexture( int dim );

///Linear buffer generation 
int genLinearBuffer( int size, GLenum format, GLuint* tex, GLuint* tbo );

//Atomic counter  generation
void genAtomicBuffer( int num, unsigned int &buffer );

//Fill a 3D texture with voxel octree leaf nodes
void octreeTo3Dtex();

//Light setting
void initLight();

#endif