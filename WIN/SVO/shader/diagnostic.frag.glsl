// Sparse Voxel Octree and Voxel Cone Tracing
// 
// University of Pennsylvania CIS565 final project
// copyright (c) 2013 Cheng-Tso Lin  

#version 430

////////////////////////////
//       ENUMERATIONS
////////////////////////////

#define	DISPLAY_DEPTH 0
#define	DISPLAY_NORMAL 1
#define	DISPLAY_POSITION 2
#define	DISPLAY_COLOR 3
#define	DISPLAY_TOTAL 4
#define	DISPLAY_LIGHTS 5

/////////////////////////////////////
// Uniforms, Attributes, and Outputs
////////////////////////////////////
uniform mat4 u_persp;
uniform mat4 u_modelview;
uniform mat4 u_lightMVP;

uniform sampler2D u_Depthtex;
uniform sampler2D u_Normaltex;
uniform sampler2D u_Positiontex;
uniform sampler2D u_Colortex;

uniform sampler2D u_shadowmap;

uniform float u_Far;
uniform float u_Near;
uniform int u_DisplayType;

uniform int u_ScreenWidth;
uniform int u_ScreenHeight;

uniform vec4 u_Light;
uniform vec3 u_LightColor;
uniform vec3 u_eyePos;
in vec2 fs_Texcoord;

out vec4 out_Color;
///////////////////////////////////////

uniform float zerothresh = 1.0f;
uniform float falloff = 0.1f;


/////////////////////////////////////
//				UTILITY FUNCTIONS
/////////////////////////////////////

//Depth used in the Z buffer is not linearly related to distance from camera
//This restores linear depth
float linearizeDepth(float exp_depth, float near, float far) {
    return	(2 * near) / (far + near -  exp_depth * (far - near)); 
}

//Helper function to automatically sample and unpack normals
vec3 sampleNrm(vec2 texcoords) {
    return texture(u_Normaltex,texcoords).xyz;
}

//Helper function to automicatlly sample and unpack positions
vec4 samplePos(vec2 texcoords) {
    vec4 pos = texture( u_Positiontex, texcoords );
    return pos;
}

//Helper function to automicatlly sample and unpack positions
vec4 sampleCol(vec2 texcoords) {
    return texture(u_Colortex,texcoords);
}

vec3 shade( vec4 light, vec3 lightColor, vec4 vertexColor, vec3 pos, vec3 N )
{
    vec3 L, H, V, R;
	float LdotN, blinnTerm;
	vec3 color;
	float decay = 1;

	N = normalize(N);
	L = light.w > 0 ? light.xyz - pos : light.xyz;
	L = normalize(L);
	V = normalize( -pos );
	H = normalize( L+V );
	
	LdotN = max( dot(L,N), 0 );
	blinnTerm = max( dot(N, H), 0 );
	blinnTerm = pow( blinnTerm, vertexColor.a );

	if( light.w > 0 )
	    decay = 1.0 / distance( light.xyz, pos ) / distance( light.xyz, pos );
	color = decay * ( vertexColor.rgb * LdotN + vec3(1,1,1)*blinnTerm );

	return color;
}

///////////////////////////////////
// MAIN
//////////////////////////////////
void main() {

    float exp_depth = texture(u_Depthtex, fs_Texcoord).r;
    float lin_depth = linearizeDepth(exp_depth,u_Near,u_Far);
	
    vec3 normal = sampleNrm(fs_Texcoord);
	vec4 color = sampleCol( fs_Texcoord );

    vec4 pos = samplePos(fs_Texcoord);

	float visibility = 1.0;
	vec4 posEyeSpace = u_modelview * pos;
	vec4 shadowCoord = u_lightMVP * pos;
	float depthBias = 0.005;
	if( textureProj( u_shadowmap, shadowCoord.xyw ).z < ( shadowCoord.z - depthBias )/shadowCoord.w  )
	    visibility = 0.1;

    out_Color = visibility*vec4( shade( u_Light, u_LightColor, color, posEyeSpace.xyz/posEyeSpace.w, normal ), 0 ); 
	
	out_Color = color;
	//float s_depth = texture( u_shadowmap, fs_Texcoord ).r;
	//out_Color = vec4( s_depth, s_depth, s_depth, 0 );
}

