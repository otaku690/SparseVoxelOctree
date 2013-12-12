// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  
# version 430

uniform mat4x4 u_mvp;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 texcoord;

void main(void) {

    gl_Position = u_mvp * vec4( Position, 1 );

}