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
layout(location = 5) in vec4 v_Pos;

layout(set = 1, binding = 1) uniform sampler2D uTexture0;
layout(set = 1, binding = 2) uniform sampler2D uBackTexture0;

layout(set = 1, binding = 0) uniform Block 
{
	uniform	vec4	g_scale;
	uniform	vec4	mUVInversedBack;
};

layout(location = 0) out vec4 o_Color;

void main() {
	if (TextureEnable)
	{
		o_Color = texture(uTexture0, v_TexCoord.xy);
	}
	else
	{
		o_Color = vec4(1.0, 1.0, 1.0, 1.0);
	}

	o_Color.a = o_Color.a * v_Color.a;

	vec2 pos = v_Pos.xy / v_Pos.w;
	vec2 posU = v_Tangent.xy / v_Tangent.w;
	vec2 posR = v_Binormal.xy / v_Binormal.w;

	float xscale = (o_Color.x * 2.0 - 1.0) * v_Color.x * g_scale.x;
	float yscale = (o_Color.y * 2.0 - 1.0) * v_Color.y * g_scale.x;

	vec2 uv = pos + (posR - pos) * xscale + (posU - pos) * yscale;

	uv.x = (uv.x + 1.0) * 0.5;
	uv.y = (uv.y + 1.0) * 0.5;
//	uv.y = 1.0 - (uv.y + 1.0) * 0.5;

	uv.y = mUVInversedBack.x + mUVInversedBack.y * uv.y;

	vec3 color = texture(uBackTexture0, uv).xyz;
	o_Color.xyz = color;
}