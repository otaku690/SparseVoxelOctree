// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

#version 430

uniform mat4x4 u_Model;
uniform mat4x4 u_View;
uniform mat4x4 u_Persp;
uniform mat4x4 u_InvTrans;

uniform vec4 kd;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 texcoord;

out vec3 fs_Normal;
out vec4 fs_Position;
out vec2 fs_texcoord;
out vec4 NDCPos;

void main(void) {
    fs_Normal = vec4(Normal,0.0f).xyz;
    vec4 world = u_Model * vec4(Position, 1.0);
    vec4 camera = u_View * world;
    fs_Position = vec4( Position, 1.0 );
	fs_texcoord = texcoord;
    gl_Position = u_Persp * camera;
	
	//Get position in NDC space, both this frame and the prvious frame
	NDCPos = gl_Position;
}