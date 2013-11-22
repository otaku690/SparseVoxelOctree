// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  
#include <glm/glm.hpp> 
#include <vector>

using namespace std;
using namespace glm;


struct Group
{
    int ibo_offset;
    int numTri;
    vec3 kd;
    vec3 ks;
    vec3 ka;
    vec3 em;
    float shininess;
    unsigned char* sampler2D;
    int sampler_w;
    int sampler_h;
    int sampler_id;
};

class ObjModel{
public:
    ObjModel();
    ~ObjModel();
public:
    float* vbo;
    int* ibo;   //index array for vertices
    int* tibo;  //index array for texture coordinate
    int* nibo;  //index array for normal vector
   
    float* nbo;
    float* cbo;
    float* tbo;

    int numVert;
    int numIdx;
    int numNrml;
    int numColor;
    int numTxcoord;

    Group* groups;
    int numGroup;
};

class objLoader
{
private:
    vector<ObjModel*> models;
public:

    objLoader();
	~objLoader();

    int load( string &filename );
    int getModelCount() const;
    const ObjModel* getModel( int idx ) const;
};