// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

# version 430

layout (location = 0) in vec4 glVertex;
layout (location = 1) in vec4 glColor;
layout (location = 2) in vec3 glNormal;

out vec3 v_normal;
out vec4 v_vertex;
out vec4 v_color;

uniform mat4 u_ModelView;
uniform mat4 u_Proj;
uniform mat3 u_Normal;

void main()
{
	v_vertex = glVertex;
	v_normal = u_Normal * glNormal;
	v_color = glColor; 
	gl_Position = u_Proj * u_ModelView * glVertex;
}