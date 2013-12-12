// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

#version 430

uniform mat4x4 u_InvTrans;
uniform float u_Far;
uniform vec3 u_Color;
uniform float u_shininess;
uniform sampler2D u_colorTex;
uniform sampler2D u_bumpTex;
uniform int u_bTextured;
uniform int u_bBump;

in vec3 fs_Normal;
in vec4 fs_Position;
in vec2 fs_texcoord;
in vec4 NDCPos;

layout (location = 0) out vec4 out_Normal;
layout (location = 1) out vec4 out_Position;
layout (location = 2) out vec4 out_Color;

void main(void)
{
    //out_Normal = vec4(normalize(fs_Normal),0.0f);
	out_Normal = u_InvTrans* vec4( fs_Normal,0 );
	if( u_bBump == 1 )
	    out_Normal = u_InvTrans*vec4(texture( u_bumpTex, fs_texcoord ).rgb, 0 );
    out_Position = vec4(fs_Position.xyz,1.0f); //Tuck position into 0 1 range
    out_Color = vec4(u_Color, u_shininess );
	if( u_bTextured == 1 )
	    out_Color.rgb = texture( u_colorTex, fs_texcoord ).rgb;
}
