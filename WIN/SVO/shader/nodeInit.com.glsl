// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin 
 
# version 430
layout (local_size_x = 8, local_size_y = 8, local_size_z = 1 ) in;

uniform int u_allocStart;
uniform int u_num;

uniform layout(binding = 0, r32ui ) coherent uimageBuffer u_octreeIdx;
uniform layout(binding = 2, r32ui ) coherent uimageBuffer u_octreeKd;

void main()
{
    uint offset;
    uint thxId = gl_GlobalInvocationID.y*1024 + gl_GlobalInvocationID.x;
	if( thxId >= u_num )
	    return;

	imageStore( u_octreeIdx, int( u_allocStart + thxId), uvec4(0,0,0,0) );
	imageStore( u_octreeKd, int( u_allocStart + thxId), uvec4(0,0,0,0) );
}