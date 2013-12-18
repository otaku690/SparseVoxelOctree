// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

# version 430
layout (local_size_x = 8, local_size_y = 8, local_size_z = 1 ) in;

uniform layout(binding = 0, r32ui ) uimageBuffer u_octreeIdx;
uniform layout(binding = 1, r32ui ) uimageBuffer u_octreeKd;

uniform layout(binding = 2, rgb10_a2ui ) uimageBuffer u_voxelPos;
uniform layout(binding = 3, rgba8 ) imageBuffer u_voxelKd;

uniform int u_voxelDim;
uniform int u_octreeLevel;
uniform int u_numVoxelFrag;

void imageAtomicRGBA8Avg( vec4 val, int coord, layout(r32ui) uimageBuffer buf );
uint convVec4ToRGBA8( vec4 val );
vec4 convRGBA8ToVec4( uint val );

void main()
{
    uint thxId = gl_GlobalInvocationID.y*1024 + gl_GlobalInvocationID.x;

    if( thxId >= u_numVoxelFrag )
        return;
    uvec4 loc = imageLoad( u_voxelPos, int(thxId) );
	int childIdx = 0;
	uint node;
	bool bFlag = true;

	uint voxelDim = u_voxelDim;
	uvec3 umin = uvec3(0,0,0);

	node = imageLoad( u_octreeIdx, childIdx ).r;

    for( int i = 0; i < u_octreeLevel; ++i )
    {
	    voxelDim /= 2;
		if( (node & 0x80000000 ) == 0 ) 
		{
		    bFlag = false;
		    break;
		}
		childIdx = int(node & 0x7FFFFFFF);  //mask out flag bit to get child idx

	    if( ( loc.x >= umin.x && loc.x < umin.x+voxelDim ) &&
		    ( loc.y >= umin.y && loc.y < umin.y+voxelDim ) &&
			( loc.z >= umin.z && loc.z < umin.z+voxelDim )
		  )
	    {
		    
		}
		else if(
            (loc.x >= umin.x+voxelDim && loc.x < umin.x + 2*voxelDim) &&
		    (loc.y >= umin.y && loc.y < umin.y+voxelDim) &&
			(loc.z >= umin.z && loc.z < umin.z+voxelDim )  
		)
		{
		    childIdx += 1;
		    umin.x = umin.x+voxelDim;
	    }
		else if(
		    (loc.x >= umin.x && loc.x < umin.x+voxelDim) &&
		    (loc.y >= umin.y && loc.y < umin.y+voxelDim) &&
			(loc.z >= umin.z + voxelDim && loc.z < umin.z + 2*voxelDim )
		)
		{
		    childIdx += 2;
			umin.z += voxelDim;
		}
		else if(
		    (loc.x >= umin.x + voxelDim && loc.x < umin.x + 2*voxelDim) &&
		    (loc.y >= umin.y && loc.y < umin.y+voxelDim) &&
			(loc.z >= umin.z + voxelDim && loc.z < umin.z + 2*voxelDim) 
		)
		{
		    childIdx += 3;
			umin.x += voxelDim;
			umin.z += voxelDim;
		}
		else if(
		    (loc.x >= umin.x && loc.x < umin.x + voxelDim) &&
		    (loc.y >= umin.y + voxelDim && loc.y < umin.y + 2*voxelDim) &&
			(loc.z >= umin.z && loc.z < umin.z + voxelDim )
		)
		{
		    childIdx += 4;
			umin.y += voxelDim;
		
		}
		else if(
		    (loc.x >= umin.x + voxelDim && loc.x < umin.x + 2*voxelDim) &&
		    (loc.y >= umin.y + voxelDim && loc.y < umin.y + 2*voxelDim) &&
			(loc.z >= umin.z && loc.z < umin.z + voxelDim) 
		)
		{
		    childIdx += 5;
			umin.x += voxelDim;
			umin.y += voxelDim;
		}
		else if(
		    (loc.x >= umin.x && loc.x < umin.x + voxelDim) &&
		    (loc.y >= umin.y + voxelDim && loc.y < umin.y + 2*voxelDim) &&
			(loc.z >= umin.z + voxelDim && loc.z < umin.z + voxelDim*2) 
		)
		{
		    childIdx += 6;
			umin.z += voxelDim;
			umin.y += voxelDim;
		}
		else if(
		    (loc.x >= umin.x + voxelDim && loc.x < umin.x + 2*voxelDim) &&
		    (loc.y >= umin.y + voxelDim && loc.y < umin.y + 2*voxelDim) &&
			(loc.z >= umin.z + voxelDim && loc.z < umin.z + voxelDim*2) 
		)
	    {
		    childIdx += 7;
			umin += voxelDim;
		}
		else
		{
		    bFlag = false;
			break;
		}
		node = imageLoad( u_octreeIdx, childIdx ).r;
	}

	vec4 color = imageLoad( u_voxelKd, int(thxId) );
	
	//Use a atomic running average method to prevent buffer saturation
	//From OpenGL Insight ch. 22
	imageAtomicRGBA8Avg( color, childIdx, u_octreeKd );
}

//UINT atomic running average method
//From OpenGL Insight ch. 22
vec4 convRGBA8ToVec4( in uint val )
{
    return vec4( float( (val&0x000000FF) ), float( (val&0x0000FF00)>>8U),
	             float( (val&0x00FF0000)>>16U), float( (val&0xFF000000)>>24U) );
}

uint convVec4ToRGBA8( in vec4 val )
{
    return ( uint(val.w)&0x000000FF)<<24U | (uint(val.z)&0x000000FF)<<16U | (uint(val.y)&0x000000FF)<<8U | (uint(val.x)&0x000000FF);
}

void imageAtomicRGBA8Avg( vec4 val, int coord, layout(r32ui) uimageBuffer buf )
{
    val.rgb *= 255.0;
	val.a = 1;

	uint newVal = convVec4ToRGBA8( val );
	uint prev = 0;
	uint cur;
	
	while( (cur = imageAtomicCompSwap( buf, coord, prev, newVal ) ) != prev )
   {
       prev = cur;
	   vec4 rval = convRGBA8ToVec4( cur );
	   rval.xyz = rval.xyz*rval.w;
	   vec4 curVal = rval +  val;
	   curVal.xyz /= curVal.w;
	   newVal = convVec4ToRGBA8( curVal );
   }

     
}