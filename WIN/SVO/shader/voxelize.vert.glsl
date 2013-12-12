// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

//Vertex shader for scene voxelization
//Using the method described in Ch. 22, OpenGL Insights

# version 430

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 texcoord;

out vec3 v_vertex;
out vec3 v_normal;
out vec2 v_texcoord;

void main()
{
    v_vertex = Position;
	v_normal = Normal;
	v_texcoord = texcoord;
    gl_Position = vec4( Position, 1 );
}