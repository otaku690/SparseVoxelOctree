// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

//Fragment shader for scene voxelization
//Using the method described in Ch. 22, OpenGL Insights

# version 430

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;

in vec3 v_vertex[];
flat out int f_axis;   //indicate which axis the projection uses

uniform mat4 u_MVPx;
uniform mat4 u_MVPy;
uniform mat4 u_MVPz;

void main()
{
	vec3 faceNormal = normalize( cross( v_vertex[1]-v_vertex[0], v_vertex[2]-v_vertex[0] ) );
	float NdotXAxis = abs( faceNormal.x );
	float NdotYAxis = abs( faceNormal.y );
	float NdotZAxis = abs( faceNormal.z );
	mat4 proj;

	if( NdotXAxis > NdotYAxis && NdotXAxis > NdotZAxis )
    {
	    proj = u_MVPx;
		f_axis = 1;
	}
	else if( NdotYAxis > NdotXAxis && NdotYAxis > NdotZAxis  )
    {
	    proj = u_MVPy;
		f_axis = 2;
    }
	else
    {
	    proj = u_MVPz;
		f_axis = 3;
	}

    gl_Position = proj*gl_in[0].gl_Position;

	EmitVertex();

	gl_Position = proj*gl_in[1].gl_Position;

	EmitVertex();

	gl_Position = proj*gl_in[2].gl_Position;

	EmitVertex();

	EndPrimitive();
}