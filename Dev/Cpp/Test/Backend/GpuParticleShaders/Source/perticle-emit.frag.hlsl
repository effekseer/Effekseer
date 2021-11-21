#include "noise.hlsli"

struct PS_INPUT {
    float4 Position : SV_POSITION;
    float3 v_Particle : TEXCOORD0;
    float3 v_Position : TEXCOORD1;
    float3 v_Direction : TEXCOORD2;
};

struct PS_OUTPUT {
    float4 o_ParticleData0 : SV_Target0;    // |   Pos X   |   Pos Y   |   Pos Z   |  Dir XYZ  |
    float4 o_ParticleData1: SV_Target1;     // | LifeCount | Lifetime  |   Seed    |  Vel XYZ  |
};

PS_OUTPUT main(PS_INPUT input) {
    PS_OUTPUT output;
    float packedDir = packVec3(normalize(input.v_Direction));
    output.o_ParticleData0 = float4(input.v_Position, packedDir);
    output.o_ParticleData1 = float4(0.0, input.v_Particle.y, input.v_Particle.z, packedDir);
    return output;
}
