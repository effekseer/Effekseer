#include "noise.hlsli"

struct VS_INPUT {
    //float  a_ParticleIndex: TEXCOORD0;
    uint instanceID : SV_InstanceID;
    float2 a_VertexPosition: TEXCOORD0;
};

struct VS_OUTPUT {
    float4 Position : SV_POSITION;
    float4 v_Color: COLOR0;
};

Texture2D ParticleData0 : register(t0); // |   Pos X   |   Pos Y   |   Pos Z   |  Dir XYZ  |
SamplerState ParticleData0Sampler : register(s0);
Texture2D ParticleData1 : register(t1); // | LifeCount | Lifetime  |   Index   |   Seed    |
SamplerState ParticleData1Sampler : register(s1);
Texture2D ColorTable : register(t2);
SamplerState ColorTableSampler : register(s2);

cbuffer CB : register(b0) {
    float4 ID2TPos;
    float4x4 ViewMatrix;
    float4x4 ProjMatrix;
};

float2 rotate(float2 pos, float deg) {
    const float toRad = 3.141592 / 180.0;
    float c = cos(deg * toRad);
    float s = sin(deg * toRad);
    return mul(pos, float2x2(c, -s, s, c));
    //return float2x2(c, -s, s, c) * pos;
}

float fadeIn(float duration, float age, float lifetime) {
    return clamp(age / duration, 0.0, 1.0);
}

float fadeOut(float duration, float age, float lifetime) {
    return clamp((lifetime - age) / duration, 0.0, 1.0);
}

float fadeInOut(float fadeinDuration, float fadeoutDuration, float age, float lifetime) {
    return fadeIn(fadeinDuration, age, lifetime) * fadeOut(fadeoutDuration, age, lifetime);
}

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;

    int particleID = input.instanceID;
    int2 ID2TPos2i = int2(ID2TPos.x,ID2TPos.y);
    int2 texPos = int2(particleID & ID2TPos2i.x, particleID >> ID2TPos2i.y);
    float4 data0 = ParticleData0.Sample(ParticleData0Sampler, float4(texPos, 0, 0));
    float4 data1 = ParticleData1.Sample(ParticleData1Sampler, float4(texPos, 0, 0));
    
    float age = data1.x;
    float lifetime = data1.y;

    if (age >= lifetime) {
        output.Position = float4(0.0,0.0,0.0,0.0);
        output.v_Color = float4(0.0,0.0,0.0,0.0);
    }
    else {
        float3 position = data0.xyz;
        position.xyz += float3(rotate(input.a_VertexPosition * 0.003, 45.0), 0.0);
        //output.Position = ProjMatrix * ViewMatrix * float4(position, 1.0);
        output.Position = float4(position, 1.0);
        output.Position = mul(output.Position, ViewMatrix);
        output.Position = mul(output.Position, ProjMatrix);
        
        float v = snoise(float3(texPos, 0) / 512.0);
        float2 texCoord = float2(v,v);
        //output.v_Color = ColorTable.Sample(ColorTableSampler, float4(texCoord, 0, 0));
        output.v_Color = ColorTable.SampleLevel(ColorTableSampler, texCoord, 0);
        output.v_Color.a *= fadeInOut(1.0, 10.0, age, lifetime);
    }

    return output;
}
