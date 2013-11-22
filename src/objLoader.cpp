// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

#include "objloader.h"
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <glm.h> 

using namespace std;

objLoader::objLoader()
{
}

int objLoader::load( string &filename )
{
    int offset = 0, gid = 0, tid =0;
    ObjModel* newMesh;
    GLMmodel *model = glmReadOBJ( (char*)filename.c_str() );
    if( model == NULL )
    {
        newMesh = NULL;
        return 0;
    }

    glmUnitize( model );

    newMesh = new ObjModel();
    memset( newMesh, 0, sizeof( ObjModel ) );
    newMesh->numVert = model->numvertices;
    newMesh->numIdx = model->numtriangles * 3;
    newMesh->numNrml = model->numnormals;
    newMesh->numTxcoord = model->numtexcoords;
   
    newMesh->vbo = new float[ newMesh->numVert * 3 ];
    newMesh->ibo = new int[ newMesh->numIdx ];
    newMesh->nibo = new int[ newMesh->numIdx ];
    newMesh->tibo = new int[ newMesh->numIdx ];

    if( newMesh->numNrml > 0 )
        newMesh->nbo = new float[ newMesh->numNrml * 3 ];

    if( newMesh->numTxcoord > 0 )
        newMesh->tbo = new float[ newMesh->numTxcoord * 2];

    GLMgroup* group = model->groups;
   

    newMesh->groups = new Group[model->numgroups];
    newMesh->numGroup = model->numgroups;
    
    //this loop copy index data
    //and copy normal and texture data in the order indicated by the index data
    while(group)
    {
        //copy material info
        memset( &newMesh->groups[gid], 0, sizeof( Group ) );
        if( model->nummaterials )
        {
            GLMmaterial* mtl = &model->materials[group->material];
            memcpy( &newMesh->groups[gid].ks[0], &mtl->specular[0], sizeof(float)*3 );
            memcpy( &newMesh->groups[gid].kd[0], &mtl->diffuse[0], sizeof(float)*3 );
            memcpy( &newMesh->groups[gid].ka[0], &mtl->ambient[0], sizeof(float)*3 );
            memcpy( &newMesh->groups[gid].em[0], &mtl->emmissive[0], sizeof(float)*3 );
            newMesh->groups[gid].shininess = mtl->shininess;
            if( mtl->texture_name != NULL )
            {
                newMesh->groups[gid].sampler2D = glmReadPPM( mtl->texture_name, &newMesh->groups[gid].sampler_w, &newMesh->groups[gid].sampler_h );
                newMesh->groups[gid].sampler_id = tid;
                tid++;
            }
            else
            {
                newMesh->groups[gid].sampler_id = -1;
                newMesh->groups[gid].sampler2D = NULL;
            }
        }
        
        newMesh->groups[gid].ibo_offset = 3*offset;
        newMesh->groups[gid].numTri = group->numtriangles;
       
        gid++;
        

        GLMtriangle* tri;

        for( unsigned int i = 0; i < group->numtriangles; ++i )
        {
            tri = &model->triangles[group->triangles[i]];

            //First we copy index data of vertices
            newMesh->ibo[3*offset] = tri->vindices[0]-1;
            newMesh->ibo[3*offset+1] = tri->vindices[1]-1;
            newMesh->ibo[3*offset+2] = tri->vindices[2]-1;

            //Then copy index data of normals
            if( newMesh->numNrml )
            {
                newMesh->nibo[3*offset] = tri->nindices[0] - 1;
                newMesh->nibo[3*offset+1] = tri->nindices[1] - 1;
                newMesh->nibo[3*offset+2] = tri->nindices[2] - 1;
            }
            //and index data of tex coords
            if( newMesh->numTxcoord )
            {
                newMesh->tibo[3*offset] = tri->tindices[0] - 1;
                newMesh->tibo[3*offset+1] = tri->tindices[1] - 1;
                newMesh->tibo[3*offset+2] = tri->tindices[2] - 1;
            }
            offset += 1;
        }

        group = group->next;
    }

    memcpy( &newMesh->vbo[0], &model->vertices[3], sizeof(float)*3* newMesh->numVert );

    if( newMesh->numNrml > 0 )
    {
        memcpy( &newMesh->nbo[0], &model->normals[3], sizeof(float)*3* newMesh->numNrml );
    }
    else
    {
        newMesh->numNrml = 1;
        newMesh->nbo = new float[3];
        newMesh->nbo[0] = newMesh->nbo[1] = newMesh->nbo[2] = 0.0f;
        memset( &newMesh->nibo[0], 0, sizeof( int ) * newMesh->numIdx );
    }


    if( newMesh->numTxcoord > 0 )
        memcpy( &newMesh->tbo[0], &model->texcoords[2], sizeof(float)*2* newMesh->numTxcoord );
    else
    {
        newMesh->numTxcoord = 1;
        newMesh->tbo = new float[2];
        newMesh->tbo[0] = newMesh->tbo[1] = 0.0f;
        memset( &newMesh->tibo[0], 0, sizeof( int ) * newMesh->numIdx );
    }


    glmDelete( model );
    cout<<newMesh->numVert<<" vertices loaded with "<<(newMesh->numIdx/3)<<" faces.\n";

    models.push_back( newMesh );
    return 1;
}

int objLoader::getModelCount() const
{
    return models.size();
}

const ObjModel* objLoader::getModel( int idx ) const
{
    try
    {
        return models.at(idx);
    }
    catch ( out_of_range &e )
    {
        cout<<e.what()<<endl;
        return NULL;
    }
}

objLoader::~objLoader()
{
    int num = models.size();
    for( vector<ObjModel*>::iterator it = models.begin();
         it != models.end(); ++it )
    {
        delete (*it);   
    }
}

ObjModel::ObjModel()
{
}

ObjModel::~ObjModel()
{
    for( int i = 0; i < numGroup; ++i )
        delete [] groups[i].sampler2D;
}