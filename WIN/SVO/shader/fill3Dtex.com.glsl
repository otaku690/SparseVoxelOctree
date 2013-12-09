// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  
# version 430

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8 ) in;
//layout (local_size_x = 64, local_size_y = 1, local_size_z = 1 ) in;

uniform layout(binding=0, r32f ) image3D u_voxelImage;
uniform layout(binding=1, r32ui ) uimageBuffer u_octreeBuf;
uniform layout(binding=2, rgb10_a2ui) uimageBuffer u_voxelPos;
uniform int u_octreeLevel;
uniform int u_voxelDim;
uniform int u_numVoxelFrag;

void main()
{
    vec4 data = vec4( 1.0, 0.0, 0.0, 0.0 );
    uvec3 umin, umax;
    uvec3 loc = gl_GlobalInvocationID;
	uint idx = 0;
	int depth;
	int voxelDim = u_voxelDim;
	
	//decide max and min coord for the root node
	umin = uvec3(0,0,0);
	umax = uvec3( voxelDim, voxelDim, voxelDim );

    for( depth = 0; depth < u_octreeLevel; ++depth )
    {
	    idx = imageLoad( u_octreeBuf, int(idx) ).r;
		idx &= 0x7FFFFFFF;  //mask out flag bit to get child pointer
		if( idx == 0 )
		    break;
		voxelDim /= 2;

	    if( loc.x >= umin.x && loc.x < umin.x+voxelDim &&
		    loc.y >= umin.y && loc.y < umin.y+voxelDim &&
			loc.z >= umin.z && loc.z < umin.z+voxelDim 
		  )
	    {
		    
		}
		else if(
            loc.x >= umin.x+voxelDim && loc.x < umin.x + 2*voxelDim &&
		    loc.y >= umin.y && loc.y < umin.y+voxelDim &&
			loc.z >= umin.z && loc.z < umin.z+voxelDim    
		)
		{
		    idx += 1;
		    umin.x = voxelDim;
	    }
		else if(
		    loc.x >= umin.x && loc.x < umin.x+voxelDim &&
		    loc.y >= umin.y && loc.y < umin.y+voxelDim &&
			loc.z >= umin.z + voxelDim && loc.z < umin.z + 2*voxelDim 
		)
		{
		    idx += 2;
			umin.z += voxelDim;
		}
		else if(
		    loc.x >= umin.x + voxelDim && loc.x < umin.x + 2*voxelDim &&
		    loc.y >= umin.y && loc.y < umin.y+voxelDim &&
			loc.z >= umin.z + voxelDim && loc.z < umin.z + 2*voxelDim 
		)
		{
		    idx += 3;
			umin.x += voxelDim;
			umin.z += voxelDim;
		}
		else if(
		    loc.x >= umin.x && loc.x < umin.x + voxelDim &&
		    loc.y >= umin.y + voxelDim && loc.y < umin.y + 2*voxelDim &&
			loc.z >= umin.z && loc.z < umin.z + voxelDim 
		)
		{
		    idx += 4;
			umin.y += voxelDim;
		
		}
		else if(
		    loc.x >= umin.x + voxelDim && loc.x < umin.x + 2*voxelDim &&
		    loc.y >= umin.y + voxelDim && loc.y < umin.y + 2*voxelDim &&
			loc.z >= umin.z && loc.z < umin.z + voxelDim 
		)
		{
		    idx += 5;
			umin.x += voxelDim;
			umin.y += voxelDim;
		}
		else if(
		    loc.x >= umin.x && loc.x < umin.x + voxelDim &&
		    loc.y >= umin.y + voxelDim && loc.y < umin.y + 2*voxelDim &&
			loc.z >= umin.z + voxelDim && loc.z < umin.z + voxelDim*2 
		)
		{
		    idx += 6;
			umin.z += voxelDim;
			umin.y += voxelDim;
		}
		else if(
		    loc.x >= umin.x + voxelDim && loc.x < umin.x + 2* voxelDim &&
		    loc.y >= umin.y + voxelDim && loc.y < umin.y + 2*voxelDim &&
			loc.z >= umin.z + voxelDim && loc.z < umin.z + voxelDim*2 
		)
	    {
		    idx += 7;
			umin += voxelDim;
		}
		else 
		{
		    break;
	    }	
	} 
	if( depth >= u_octreeLevel )
	    imageStore( u_voxelImage, ivec3(loc.xyz) , data );
}

//void main()
//{
//    uint thxIdx = gl_GlobalInvocationID.x;
//	if( thxIdx >= u_numVoxelFrag )
//	    return;

//    uvec4 texcoord = imageLoad( u_voxelPos, int(thxIdx) );

//	imageStore( u_voxelImage, ivec3(texcoord.xyz), vec4(1,0,0,0) );
//}