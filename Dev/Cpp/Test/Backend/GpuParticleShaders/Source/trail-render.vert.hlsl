#include "noise.hlsli"
#include "noise2D.hlsli"
#include "render-utils.hlsli"

struct VS_INPUT {
    uint instanceID : SV_InstanceID;
    float2 a_VertexPosition: TEXCOORD0;
};

struct VS_OUTPUT {
    float4 Position : SV_POSITION;
    float4 v_Color: COLOR0;
};

Texture2D ParticleData0 : register(t0);
SamplerState ParticleData0Sampler : register(s0);
Texture2D ParticleData1 : register(t1);
SamplerState ParticleData1Sampler : register(s1);
Texture2D ColorTable : register(t2);
SamplerState ColorTableSampler : register(s2);
Texture2DArray Histories : register(t3);
SamplerState HistoriesSampler : register(s3);

cbuffer CB : register(b0) {
    float4 ID2TPos;
    float4x4 ViewMatrix;
    float4x4 ProjMatrix;
	float4 Trail;			// .x=trailOffset, .y=TrailBufferSize
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
	
	int particleID = input.instanceID;
	int2 ID2TPos2i = int2(ID2TPos.x, ID2TPos.y);
	int2 texPos = int2(particleID & ID2TPos2i.x, particleID >> ID2TPos2i.y);
	//float4 data0 = ParticleData0.Sample(ParticleData0Sampler, float4(texPos, 0, 0));
	//float4 data1 = ParticleData1.Sample(ParticleData1Sampler, float4(texPos, 0, 0));
 	float4 data0 = ParticleData0.SampleLevel(ParticleData0Sampler, (float2)texPos, 0);
 	float4 data1 = ParticleData1.SampleLevel(ParticleData1Sampler, (float2)texPos, 0);
	
	float age = data1.x;
	float lifetime = data1.y;

	if (age >= lifetime || age <= 0.0) {
		output.Position = float4(0.0, 0.0, 0.0, 0.0);
		output.v_Color = float4(0.0, 0.0, 0.0, 0.0);
	} else {
		float historyID = input.a_VertexPosition.x * min(float(Trail.y), age);
		float3 position, direction;
		if (historyID >= 1.0) {
			int texIndex = (int(Trail.x) + int(historyID) - 1) % int(Trail.y);
			//float4 trailData = texelFetch(Histories, ivec3(texPos, texIndex), 0);
			//float4 trailData = Histories.Sample(HistoriesSampler, float4(texPos, texIndex, 0));
   			float4 trailData = Histories.SampleLevel(HistoriesSampler, float3(texPos, texIndex), 0);
			position = trailData.xyz;
			direction = unpackVec3(trailData.w);
		} else {
			position = data0.xyz;
			direction = unpackVec3(data0.w);
		}
		float width = 0.05;
		float3 vertex = cross(float3(0.0, 1.0, 0.0), direction) * input.a_VertexPosition.y * width;
		
		//float c = dot(float3(1.0, 0.0, 0.0), direction);
		//float s = sqrt(1.0 - c * c);
		//output.Position = ProjMatrix * (ViewMatrix * float4(position.xyz + vertex, 1.0));
		output.Position = float4(position.xyz + vertex, 1.0);
		output.Position = mul(output.Position, ViewMatrix);
		output.Position = mul(output.Position, ProjMatrix);
		
		float2 texCoord = float2(snoise(float2(texPos) / 512.0));
		//output.v_Color = ColorTable.Sample(ColorTableSampler, float4(texCoord, 0, 0));
 	 	output.v_Color = ColorTable.SampleLevel(ColorTableSampler, texCoord, 0);
		output.v_Color.a *= 0.5 * fadeInOut(10.0, 10.0, age, lifetime);
	}

	return output;
}
