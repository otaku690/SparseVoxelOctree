// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

//Vertex shader for scene voxelization
//Using the method described in Ch. 22, OpenGL Insights

# version 430

layout (location = 0) in vec4 glVertex;
layout (location = 1) in vec4 glColor;
layout (location = 2) in vec3 glNormal;

out vec3 v_vertex;
void main()
{
    v_vertex = glVertex.xyz;
    gl_Position =  glVertex;
}