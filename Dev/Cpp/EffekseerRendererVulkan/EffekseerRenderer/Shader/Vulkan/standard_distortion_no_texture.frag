#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vaColor;
layout(location = 1) in vec4 vaTexCoord;
layout(location = 2) in vec4 vaPos;
layout(location = 3) in vec4 vaPosR;
layout(location = 4) in vec4 vaPosU;

layout(set = 1, binding = 1) uniform sampler2D uBackTexture0;

layout(set = 1, binding = 0) uniform Block 
{
	uniform	vec4 g_scale;
};

layout(location = 0) out vec4 o_Color;

void main() {
	vec4 color = vaColor;
	color.xyz = vec3(1.0,1.0,1.0);

	vec2 pos = vaPos.xy / vaPos.w;
	vec2 posU = vaPosU.xy / vaPosU.w;
	vec2 posR = vaPosR.xy / vaPosR.w;

	vec2 uv = pos + (posR - pos) * (color.x * 2.0 - 1.0) * vaColor.x * g_scale.x + (posU - pos) * (color.y * 2.0 - 1.0) * vaColor.y * g_scale.x;
	uv.x = (uv.x + 1.0) * 0.5;
	uv.y = (uv.y + 1.0) * 0.5;
	//uv.y = 1.0 - (uv.y + 1.0) * 0.5;

	color.xyz = texture(uBackTexture0, uv).xyz;
	
	o_Color = color;
}
