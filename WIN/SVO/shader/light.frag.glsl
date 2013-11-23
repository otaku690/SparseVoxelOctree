// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

# version 400

in vec4 v_color;
in vec4 v_vertex;
in vec3 v_normal;
layout (location = 0) out vec4 gl_FragColor;

uniform vec4 lightposn;
uniform vec4 lightColor;

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform vec4 emission;
uniform float shininess;

void main()
{
    vec3 eyeRay = normalize( -v_vertex.xyz );
	vec3 N  = normalize( v_normal );

	vec3 L = normalize( lightposn.xyz - v_vertex.xyz );
	vec3 H = normalize( L + eyeRay );
	vec4 color = lightColor * ( diffuse * max( dot( v_normal, L ), 0 ) + specular * pow( max( dot( N, H ), 0 ), shininess ) );

	gl_FragColor = color + emission + shininess;
	gl_FragColor += vec4( 1, 1, 1, 1 );
}