// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

# version 400

//layout ( triangles ) in;
//layout ( triangle_strip, max_vertices = 3 ) out;
layout ( points ) in;
//layout ( line_strip, max_vertices = 26 ) out;
layout ( points, max_vertices = 1 ) out;
in vec4 v_vertex[];
in vec4 v_color[];
in vec3 v_normal[];

out vec4 f_vertex;
out vec4 f_color;
out vec3 f_normal;

//uniform mat4 u_MVPx;
//uniform mat4 u_MVPy;
//uniform mat4 u_MVPz;
uniform mat4 u_ModelView;
uniform mat4 u_Proj;
uniform mat3 u_Normal;
uniform float u_halfDim;
uniform sampler3D u_voxel;

void main()
{
	vec4 pos, color;
	float density = texture(  u_voxel, v_vertex[0].xyz ).r;
	if( density > 0.0 )
	    color = vec4( 1, 1, 1, 0 );
    else
	    color = vec4( 0, 0, 0, 0 );
	f_vertex = v_vertex[0];
	f_color = color;
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	//+Z front face
	//pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
 //   gl_Position = u_Proj * u_ModelView * pos;
	//f_vertex = v_vertex[0];
	//f_normal = v_normal[0];
	//f_color = color;
	//EmitVertex();

	//pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
 //   gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();

 //   pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
 //   gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();

	//pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
 //   gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();

	////+X
	////for degenerate purpose
	//EmitVertex();

	//pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();
	//pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();
	//pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();
	

	////-Z
	//EmitVertex(); //for degenerate purpose

	//pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();
	//pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();
	//pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();

	////-X
	//EmitVertex(); //for degenerate purpose

	//pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();
	//pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();
	//pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();

	////-Y
	//EmitVertex();

	//pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();
	//pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();
	//pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y - u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();

	////+Y
	//EmitVertex();
	
	//pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();
	//EmitVertex();

	//pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z - u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();
	//pos = vec4( v_vertex[0].x + u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();
	//pos = vec4( v_vertex[0].x - u_halfDim, v_vertex[0].y + u_halfDim, v_vertex[0].z + u_halfDim, v_vertex[0].w );
	//gl_Position = u_Proj * u_ModelView * pos;
	//EmitVertex();

	//EmitVertex();


	EndPrimitive();
}