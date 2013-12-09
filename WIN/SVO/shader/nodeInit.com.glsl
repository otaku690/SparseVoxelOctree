// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin 
 
# version 430
layout (local_size_x = 64, local_size_y = 1, local_size_z = 1 ) in;

uniform int u_allocStart;
uniform int u_num;

uniform layout(binding = 0, r32ui ) uimageBuffer u_octreeBuf;

void main()
{
    uint offset;
    uint thxId = gl_GlobalInvocationID.x;
	if( thxId >= u_num )
	    return;

	imageStore( u_octreeBuf, int( u_allocStart + thxId), uvec4(0,0,0,0) );
}