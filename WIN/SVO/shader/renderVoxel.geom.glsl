// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

# version 430

layout ( points ) in;
//layout ( line_strip, max_vertices = 16) out;
layout ( triangle_strip, max_vertices = 26 ) out;
//layout ( points, max_vertices = 1 ) out;
in vec4 v_vertex[];
in vec4 v_color[];
in uvec3 v_texcoord[];
in vec3 v_normal[];

out vec4 f_vertex;
out vec4 f_color;
out vec3 f_normal;

uniform mat4 u_ModelView;
uniform mat4 u_Proj;
uniform float u_halfDim;
uniform int u_voxelDim;
uniform int u_octreeLevel;

//uniform sampler3D u_voxel;

uniform layout(binding=0, r32ui ) uimageBuffer u_octreeBuf;
uniform layout(binding=1, r32ui ) uimageBuffer u_octreeKd;
//uniform usamplerBuffer u_octreeIdx;
//uniform usamplerBuffer u_octreeKd;

bool nodeOccupied( in uvec3 loc, out int leafIdx );

vec4 convRGBA8ToVec4( uint val );

//void main()
//{
//	vec4 pos, color;
//	//vec3 coord = vec3(v_texcoord[0].xyz);
//	//coord /= u_voxelDim;
//	//float density = texture(  u_voxel, coord).r;
//	//if( density > 0.0 )
//	//   f_color = vec4( 1,1,1,0 );
// //   else
//	//   return;
//	int leafIdx;
//	if( nodeOccupied( v_texcoord[0], leafIdx )  )
//	{
//	    uint val =  imageLoad( u_octreeKd, leafIdx ).r;
//	    f_color =  convRGBA8ToVec4(val)/2552 ; 
//	}
//	else
//	    return;


//	//+Z front face
	
//	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	//f_color = color;
//	EmitVertex();

//	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//    pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//	//-x face
//	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//	////+Y face
//	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//	////-Z face
//	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//	////+X face

//	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
//    gl_Position = u_Proj * u_ModelView * pos;
//	EmitVertex();

//	EndPrimitive();
//}

vec4 convRGBA8ToVec4( uint val )
{
    return vec4( float( (val&0x000000FF) ), float( (val&0x0000FF00)>>8U),
	             float( (val&0x00FF0000)>>16U), float( (val&0xFF000000)>>24U) );
}

bool nodeOccupied( in uvec3 loc, out  int leafIdx )
{
    int voxelDim = u_voxelDim;

	//decide max and min coord for the root node
	uvec3 umin = uvec3(0,0,0);

    bool bOccupied = true;
	uint idx;
	uvec3 offset;
	int depth;
	
    for( depth = 0; depth <= u_octreeLevel; ++depth )
    {
	    leafIdx = int(idx);
	    idx = imageLoad( u_octreeBuf, int(idx) ).r;
		//idx = texelFetch( u_octreeIdx, int(idx) ).r;
		if( (idx & 0x80000000) == 0 )
	    {
		    bOccupied= false;
		    break;
		}
		else if( depth == u_octreeLevel )
		{
		    
		    break;
		}

		idx &= 0x7FFFFFFF;  //mask out flag bit to get child pointer
		if( idx == 0 )
		{
		    bOccupied = false;
		    break;
		}
		voxelDim /= 2;

	 //   if( (loc.x >= umin.x && loc.x < umin.x+voxelDim) &&
		//    (loc.y >= umin.y && loc.y < umin.y+voxelDim) &&
		//	(loc.z >= umin.z && loc.z < umin.z+voxelDim) 
		//  )
	 //   {
		    
		//}
		//else if(
  //          (loc.x >= umin.x+voxelDim && loc.x < umin.x + 2*voxelDim) &&
		//    (loc.y >= umin.y && loc.y < umin.y+voxelDim) &&
		//	(loc.z >= umin.z && loc.z < umin.z+voxelDim)    
		//)
		//{
		//    idx += 1;
		//    umin.x += voxelDim;
	 //   }
		//else if(
		//    (loc.x >= umin.x && loc.x < umin.x+voxelDim) &&
		//    (loc.y >= umin.y && loc.y < umin.y+voxelDim) &&
		//	(loc.z >= umin.z + voxelDim && loc.z < umin.z + 2*voxelDim) 
		//)
		//{
		//    idx += 2;
		//	umin.z += voxelDim;
		//}
		//else if(
		//    (loc.x >= umin.x + voxelDim && loc.x < umin.x + 2*voxelDim) &&
		//    (loc.y >= umin.y && loc.y < umin.y+voxelDim) &&
		//	(loc.z >= umin.z + voxelDim && loc.z < umin.z + 2*voxelDim) 
		//)
		//{
		//    idx += 3;
		//	umin.x += voxelDim;
		//	umin.z += voxelDim;
		//}
		//else if(
		//    (loc.x >= umin.x && loc.x < umin.x + voxelDim) &&
		//    (loc.y >= umin.y + voxelDim && loc.y < umin.y + 2*voxelDim) &&
		//	(loc.z >= umin.z && loc.z < umin.z + voxelDim) 
		//)
		//{
		//    idx += 4;
		//	umin.y += voxelDim;
		
		//}
		//else if(
		//    (loc.x >= umin.x + voxelDim && loc.x < umin.x + 2*voxelDim) &&
		//    (loc.y >= umin.y + voxelDim && loc.y < umin.y + 2*voxelDim) &&
		//	(loc.z >= umin.z && loc.z < umin.z + voxelDim )
		//)
		//{
		//    idx += 5;
		//	umin.x += voxelDim;
		//	umin.y += voxelDim;
		//}
		//else if(
		//    (loc.x >= umin.x && loc.x < umin.x + voxelDim) &&
		//    (loc.y >= umin.y + voxelDim && loc.y < umin.y + 2*voxelDim) &&
		//	(loc.z >= umin.z + voxelDim && loc.z < umin.z + voxelDim*2) 
		//)
		//{
		//    idx += 6;
		//	umin.z += voxelDim;
		//	umin.y += voxelDim;
		//}
		//else if(
		//    (loc.x >= umin.x + voxelDim && loc.x < umin.x + 2* voxelDim) &&
		//    (loc.y >= umin.y + voxelDim && loc.y < umin.y + 2*voxelDim) &&
		//	(loc.z >= umin.z + voxelDim && loc.z < umin.z + voxelDim*2 )
		//)
	 //   {
		//    idx += 7;
		//	umin += voxelDim;
		//}
		//else 
		//{
		//    bOccupied = false;
		//    break;
	 //   }	

	    //offset.x = clamp( int(1 + loc.x - umin.x - voxelDim), 0, 1 );
	    //offset.y = clamp( int(1 + loc.y - umin.y - voxelDim), 0, 1 );
	    //offset.z = clamp( int(1 + loc.z - umin.z - voxelDim), 0, 1 );
		offset = clamp( ivec3( 1 + loc - umin - voxelDim ), 0, 1 );
	    idx += offset.x + 4*offset.y + 2*offset.z;
	
	    umin += voxelDim * offset;
	    //umin.x += voxelDim * offset.x;
	    //umin.y += voxelDim * offset.y;
	    //umin.z += voxelDim * offset.z;
	}

	return bOccupied;
}

void main()
{
	vec4 pos, color;
	int leafIdx;
	if( nodeOccupied( v_texcoord[0], leafIdx )  )
	{
	    //uint val = texelFetch( u_octreeKd,leafIdx ).r;
	    uint val =  imageLoad( u_octreeKd, leafIdx ).r;
	    f_color =  convRGBA8ToVec4(val).rgba/255 ; 
	}
	else
	    return;


	//+Z front face
	//f_color = vec4(0.5, 0.5, 0.5, 1 );
	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
    gl_Position = u_Proj * u_ModelView * pos;
	f_vertex = v_vertex[0];
	f_normal = v_normal[0];
	//f_color = color;
	EmitVertex();

	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
    gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();

    pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
    gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();

	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
    gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();

	//+X
	//for degenerate purpose
	EmitVertex();
	//f_color = vec4( 0.2, 0.2, 0.2, 1 );
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();
	

	//-Z
	EmitVertex(); //for degenerate purpose

	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();

	//-X
	EmitVertex(); //for degenerate purpose
	//f_color = vec4( 0.5, 0.5, 0.5, 1 );
	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();

	//-Y
	EmitVertex();

	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();

	//+Y
	EmitVertex();
	//f_color = color;
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();
	EmitVertex();

	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();
	pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	gl_Position = u_Proj * u_ModelView * pos;
	EmitVertex();

	EmitVertex();

	EndPrimitive();
}