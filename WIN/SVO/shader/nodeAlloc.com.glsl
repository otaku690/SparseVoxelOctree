// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  
# version 430
layout (local_size_x = 64, local_size_y = 1, local_size_z = 1 ) in;

uniform int u_start;
uniform int u_allocStart;
uniform int u_num;

uniform layout( binding = 0, r32ui ) uimageBuffer u_octreeBuf;

//atomic counter 
layout ( binding = 0, offset = 0 ) uniform atomic_uint u_allocCount;

void main()
{
    uint offset;
    uint thxId = gl_GlobalInvocationID.x;
	if( thxId >= u_num )
	    return;

	//get child pointer
	uint childIdx = imageLoad( u_octreeBuf, u_start + int(thxId) ).r;

	if( (childIdx & 0x80000000 ) != 0 ) //need to allocate
	{
	    offset = atomicCounterIncrement( u_allocCount );
		offset *= 8; //one tile has eight nodes
		offset += u_allocStart; //Add allocation offset 
		offset |= 0x80000000;    //Set the most significant bit
		imageStore( u_octreeBuf, u_start + int(thxId), uvec4(offset,0,0,0) );
    }
}