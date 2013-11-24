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

uniform  layout(size1x32) image3D u_voxelImage;
uniform int u_width;
uniform int u_height;

void main()
{
    vec4 data = vec4(1.0,1.0,1.0,1.0);
    //ivec3 temp = ivec3( gl_FragCoord.x, gl_FragCoord.y, u_width * gl_FragCoord.z ) ;
	ivec3 temp = ivec3( gl_FragCoord.y, gl_FragCoord.x,0 ) ;
	ivec3 texcoord;
	if( f_axis == 1 )
	{
	    texcoord.x = u_width - temp.z;
		texcoord.z = temp.x;
	}
	else if( f_axis == 2 )
    {
	    texcoord.z = temp.y;
		texcoord.y = u_height - temp.z;
	}
	else
	    texcoord = temp;
	
	imageStore( u_voxelImage, texcoord, data );
	//gl_FragColor = vec4( 1, 1, 1, 1 );
}