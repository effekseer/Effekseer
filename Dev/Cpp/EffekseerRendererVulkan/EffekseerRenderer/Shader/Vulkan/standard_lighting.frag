#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) centroid in vec4 v_VColor;
layout(location = 1) centroid in vec2 v_UV1;
layout(location = 2) in vec2 v_UV2;
layout(location = 3) in vec3 v_WorldP;
layout(location = 4) in vec3 v_WorldN;
layout(location = 5) in vec3 v_WorldT;
layout(location = 6) in vec3 v_WorldB;
layout(location = 7) in vec2 v_ScreenUV;


layout( set = 1,binding = 1 )  uniform sampler2D ColorTexture;
layout( set = 1,binding = 2 )  uniform sampler2D NormalTexture;

layout(set = 1,binding = 0) uniform Block 
{
	vec4 LightDirection;
	vec4 LightColor;
	vec4 LightAmbient;
};

layout(location = 0) out vec4 o_Color;

void main()
{	
	vec3 texNormal = (texture(NormalTexture, v_UV1.xy).xyz - 0.5) * 2.0;
	mat3 normalMatrix = mat3(v_WorldT.xyz, v_WorldB.xyz, v_WorldN.xyz );
	vec3 localNormal = normalize( normalMatrix * texNormal );
	float diffuse = max(0.0, dot(localNormal, LightDirection.xyz));
	
	o_Color = v_VColor * texture(ColorTexture, v_UV1.xy);
	o_Color.xyz = o_Color.xyz * (LightColor.xyz * diffuse + LightAmbient.xyz);

	if(o_Color.w <= 0.0)
		discard;
}
