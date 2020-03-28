#version 450
#extension GL_ARB_separate_shader_objects : enable

#define TextureEnable true
#define LightingEnable true
#define NormalMapEnable true

layout(location = 0) in vec4 v_Normal;
layout(location = 1) in vec4 v_Binormal;
layout(location = 2) in vec4 v_Tangent;
layout(location = 3) in vec4 v_TexCoord;
layout(location = 4) in vec4 v_Color;

layout(set = 1, binding = 1) uniform sampler2D ColorTexture;
layout(set = 1, binding = 2) uniform sampler2D NormalTexture;

layout(set = 1, binding = 0) uniform Block 
{
	uniform vec4 LightDirection;
	uniform vec4 LightColor;
	uniform vec4 LightAmbient;
};

layout(location = 0) out vec4 o_Color;

void main()
{
	o_Color = v_Color * texture(ColorTexture, v_TexCoord.xy);

	if (LightingEnable)
	{
		vec3 texNormal = (texture(NormalTexture, v_TexCoord.xy).xyz - 0.5) * 2.0;
		mat3 normalMatrix = mat3(v_Tangent.xyz, v_Binormal.xyz, v_Normal.xyz );
		vec3 localNormal = normalize( normalMatrix * texNormal );
		float diffuse = max(0.0, dot(localNormal, LightDirection.xyz));
		o_Color.xyz = o_Color.xyz * (LightColor.xyz * diffuse + LightAmbient.xyz);
	}
}


