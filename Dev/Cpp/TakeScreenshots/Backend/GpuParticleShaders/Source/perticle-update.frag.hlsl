#include "noise.hlsli"

Texture2D i_ParticleData0 : register(t0); // |   Pos X   |   Pos Y   |   Pos Z   |  Dir XYZ  |
SamplerState i_ParticleData0Sampler : register(s0);
Texture2D i_ParticleData1 : register(t1); // | LifeCount | Lifetime  |   Index   |   Seed    |
SamplerState i_ParticleData1Sampler : register(s1);

cbuffer CB : register(b0) {
	uniform float4 DeltaTime;
	//uniform float4 Flags;
};

struct PS_INPUT {
    float4 Position: SV_POSITION; // for disable DX11 validation error.
	float2 ScreenUV: TEXCOORD0;
};

struct PS_OUTPUT {
    float4 o_ParticleData0 : SV_Target0;	// |   Pos X   |   Pos Y   |   Pos Z   |  Dir XYZ  |
    float4 o_ParticleData1: SV_Target1;		// | LifeCount | Lifetime  |   Index   |   Seed    |
};

float3 orbit(float3 position, float3 direction, float3 move) {
	float3 offset = float3(0.0, 0.0, 0.0);
	float3 axis = normalize(float3(0.0, 1.0, 0.0));
	float3 diff = position - offset;
	float distance = length(diff);
	float3 normalDir;
	float radius;
	if (distance < 0.0001) {
		radius = 0.0001;
		normalDir = direction;
	} else {
		float3 normal = diff - axis * dot(axis, normalize(diff)) * distance;
		radius = length(normal);
		if (radius < 0.0001) {
			normalDir = direction;
		} else {
			normalDir = normalize(normal);
		}
	}

	//float nextRadius = 0.1;
	float nextRadius = max(0.0001, radius + move.z);
	float3 orbitDir = cross(axis, normalDir);
	float arc = 2.0 * 3.141592 * radius;
	float rotation = move.x / arc;

	float3 rotationDir = orbitDir * sin(rotation) - normalDir * (1.0 - cos(rotation));
	float3 velocity = rotationDir * radius + (rotationDir * 2.0 + normalDir) * radius * (nextRadius - radius);
	
	return velocity + axis * move.y;


	//float orbitLength = 2.0 * radius * 3.141592;
	//return orbitDir * 0.1 - normalDir * 0.01;
}

float4 myTexelFetch(Texture2D t, SamplerState s, int2 pos) {
	return t.Sample(s, float4(pos.x, pos.y, 0, 0));	// TODO: pixel to uv
}

PS_OUTPUT main(PS_INPUT input) {
	PS_OUTPUT output;

	// Load data
	//float4 data0 = myTexelFetch(i_ParticleData0, i_ParticleData0Sampler, int2(gl_FragCoord.xy), 0);
	//float4 data1 = myTexelFetch(i_ParticleData1, i_ParticleData1Sampler, int2(gl_FragCoord.xy), 0);
	float4 data0 = i_ParticleData0.Sample(i_ParticleData0Sampler, float4(input.ScreenUV, 0, 0));
	float4 data1 = i_ParticleData1.Sample(i_ParticleData1Sampler, float4(input.ScreenUV, 0, 0));
	float3 position = data0.xyz;
	float3 direction = unpackVec3(data1.w);
	
	float DeltaTime1f = DeltaTime.x;
	// Apply aging
	data1.x += DeltaTime1f;
	float lifetimeRatio = data1.x / data1.y;
	
	// Clculate velocity
	float3 velocity = float3(0.0,0.0,0.0);
	//if (Flags.x) {
		velocity += direction * lerp(0.01, 0.0, lifetimeRatio);
	//}
	//if (Flags.y) {
	//	float3 orbitMove = mix(float3(0.01, 0.0, 0.0), float3(0.5, 0.01, -0.01), lifetimeRatio);
	//	velocity += orbit(position, direction, orbitMove);
	//}
	//if (Flags.z) {
		velocity += 0.01 * noise3(position);
	//}
	//if (Flags.w) {
		//velocity += float3(0.0, -0.0001, 0.0) * data1.x;
		float3 targetPosition = float3(0.2, 0.0, 0.0);
		float3 diff = targetPosition - position;
		velocity += normalize(diff) * 0.01;
	//}

	// Update position
	position += velocity;
	
	// Update direction
	direction = (length(velocity) < 0.0001) ? direction : normalize(velocity);
	
	// Store data
	output.o_ParticleData0 = float4(position, packVec3(direction));
	output.o_ParticleData1 = data1;

	return output;
}
