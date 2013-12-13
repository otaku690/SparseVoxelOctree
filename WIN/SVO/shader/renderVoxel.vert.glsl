// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

# version 430

layout (location = 0) in vec4 glVertex;
layout (location = 1) in vec4 glColor;
layout (location = 2) in vec3 glNormal;

out vec3 v_normal;
out vec4 v_vertex;
out uvec3 v_texcoord;
out vec4 v_color;

uniform mat4 u_ModelView;
uniform mat4 u_Proj;
uniform mat3 u_Normal;
uniform int u_voxelDim;

void main()
{
   
    //v_texcoord = glVertex.xyz/u_voxelDim;
	v_texcoord.x = gl_VertexID % u_voxelDim;
	v_texcoord.z = (gl_VertexID / u_voxelDim) % u_voxelDim;
	v_texcoord.y = gl_VertexID / (u_voxelDim*u_voxelDim);
	gl_Position = u_Proj * u_ModelView * vec4( v_texcoord, 1.0 );

	//v_texcoord.xyz /= u_voxelDim;
	//v_vertex = vec4( v_texcoord*2-1, 1.0 );
	v_vertex = vec4( v_texcoord/float(u_voxelDim)*2.0-1, 1.0 );
	v_vertex.z += 1.0/u_voxelDim;
	v_vertex.x -= 1.0/u_voxelDim;
	v_normal = u_Normal * glNormal;
	v_color = glColor; 
	
}