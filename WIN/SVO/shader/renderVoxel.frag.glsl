// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

# version 430

in vec4 f_vertex;
in vec4 f_color;
in vec3 f_normal;
layout (location = 0) out vec4 gl_FragColor;

uniform vec4 lightposn;
uniform vec4 lightColor;

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform vec4 emission;
uniform float shininess;

uniform sampler3D u_voxel;

void main()
{
	gl_FragColor = f_color;
}