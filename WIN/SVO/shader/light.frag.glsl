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
 //   vec3 eyeRay = normalize( -f_vertex.xyz );
	//vec3 N  = normalize( f_normal );

	//vec3 L = normalize( lightposn.xyz - f_vertex.xyz );
	//vec3 H = normalize( L + eyeRay );
	//vec4 color = lightColor * ( diffuse * max( dot( f_normal, L ), 0 ) + specular * pow( max( dot( N, H ), 0 ), shininess ) );

	//gl_FragColor = color + emission + shininess;
	gl_FragColor = f_color;
}