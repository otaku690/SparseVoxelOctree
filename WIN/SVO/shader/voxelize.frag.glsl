// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

//Fragment shader for scene voxelization
//Using the method described in Ch. 22, OpenGL Insights

# version 430

flat in int f_axis;   //indicate which axis the projection uses
flat in vec4 f_AABB;

in vec3 f_normal;
in vec2 f_texcoord;
in vec3 f_pos;

layout (location = 0) out vec4 gl_FragColor;
layout (pixel_center_integer) in vec4 gl_FragCoord;

//atomic counter 
layout ( binding = 0, offset = 0 ) uniform atomic_uint u_voxelFragCount;

uniform layout(binding = 0, rgb10_a2ui) uimageBuffer u_voxelPos;
uniform layout(binding = 1, rgba8 ) imageBuffer u_voxelKd;
uniform layout(binding = 2, rgba16f) imageBuffer u_voxelNrml;

uniform vec3 u_Color;
uniform float u_shininess;
uniform sampler2D u_colorTex;
uniform sampler2D u_bumpTex;
uniform int u_bTextured;
uniform int u_bBump;

uniform int u_width;
uniform int u_height;
uniform int u_bStore; //0 for counting the number of voxel fragments
                      //1 for storing voxel fragments
void main()
{
    if( f_pos.x < f_AABB.x || f_pos.y < f_AABB.y || f_pos.x > f_AABB.z || f_pos.y > f_AABB.w )
	   discard ;

    vec4 data = vec4(1.0,0.0,0.0,0.0);
    //ivec3 temp = ivec3( gl_FragCoord.x, gl_FragCoord.y, u_width * gl_FragCoord.z ) ;
	uvec4 temp = uvec4( gl_FragCoord.x, gl_FragCoord.y, u_width * gl_FragCoord.z, 0 ) ;
	uvec4 texcoord;
	if( f_axis == 1 )
	{
	    texcoord.x = u_width - temp.z;
		texcoord.z = temp.x;
		texcoord.y = temp.y;
	}
	else if( f_axis == 2 )
    {
	    texcoord.z = temp.y;
		texcoord.y = u_width-temp.z;
		texcoord.x = temp.x;
	}
	else
	    texcoord = temp;

	uint idx = atomicCounterIncrement( u_voxelFragCount );
	if( u_bStore == 1 )
	{
	   vec3 N, C;
	   if( u_bBump == 1 )
	       N = texture( u_bumpTex, f_texcoord ).rgb;
	   else
	       N = f_normal;

	   if( u_bTextured == 1 )
	      C = texture( u_colorTex, f_texcoord ).rgb;
	   else
	      C = u_Color;

	   imageStore( u_voxelPos, int(idx), texcoord );
	   imageStore( u_voxelNrml, int(idx), vec4(N,0) );
	   imageStore( u_voxelKd, int(idx), vec4( C, 0 ) );
	}

	//imageStore( u_voxelImage, texcoord, data );
	//gl_FragColor = vec4( 1, 1, 1, 1 );
}