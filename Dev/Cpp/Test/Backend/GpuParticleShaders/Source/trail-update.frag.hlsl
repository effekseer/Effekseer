#include "noise.hlsli"

Texture2D i_Position : register(t0);
SamplerState i_PositionSampler : register(s0);
Texture2D i_Velocity : register(t1);
SamplerState i_VelocitySampler : register(s1);

struct PS_INPUT {
	float2 ScreenUV: TEXCOORD0;
};

struct PS_OUTPUT {
    float4 o_Histories: COLOR0;
};

PS_OUTPUT main(PS_INPUT input) {
	PS_OUTPUT output;

	// Load data
	float4 position = i_Position.Sample(i_PositionSampler, float4(input.ScreenUV, 0, 0));
	float4 velocity = i_Velocity.Sample(i_VelocitySampler, float4(input.ScreenUV, 0, 0));
	
	// Store data
	output.o_Histories = float4(position.xyz, packVec3(normalize(velocity.xyz)));

	return output;
}
