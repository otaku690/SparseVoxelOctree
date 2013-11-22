// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

# version 400

layout (location = 0) in vec4 glVertex;
layout (location = 1) in vec4 glColor;
layout (location = 2) in vec3 glNormal;

out vec3 v_normal;
out vec4 v_vertex;
out vec4 v_color;

uniform mat4 u_ModelView;
uniform mat4 u_Proj;
uniform mat4 u_Normal;

void main()
{
    gl_Position = u_Proj * u_ModelView * glVertex;
	v_vertex = u_Modelview * glVertex;
	v_normal = u_Normal * glNormal;
	v_color = glColor; 
}