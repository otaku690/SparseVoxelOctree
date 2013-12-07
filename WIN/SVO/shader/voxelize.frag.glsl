// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

//Fragment shader for scene voxelization
//Using the method described in Ch. 22, OpenGL Insights

# version 430

flat in int f_axis;   //indicate which axis the projection uses
layout (location = 0) out vec4 gl_FragColor;
layout (pixel_center_integer) in vec4 gl_FragCoord;

//atomic counter 
layout ( binding = 0, offset = 0 ) uniform atomic_uint u_voxelFragCount;

uniform layout(rgb10_a2ui) uimageBuffer u_voxelPos;
uniform int u_width;
uniform int u_height;
uniform int u_bStore; //0 for counting the number of voxel fragments
                      //1 for storing voxel fragments
void main()
{
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
	    imageStore( u_voxelPos, int(idx), texcoord );

	//imageStore( u_voxelImage, texcoord, data );
	//gl_FragColor = vec4( 1, 1, 1, 1 );
}