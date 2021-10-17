#include "noise.hlsli"

struct PS_INPUT {
    float3 v_Particle;
    float3 v_Position;
    float3 v_Direction;
};

struct PS_OUTPUT {
    float4 o_ParticleData0 : COLOR0;    // |   Pos X   |   Pos Y   |   Pos Z   |  Dir XYZ  |
    float4 o_ParticleData1: COLOR1;     // | LifeCount | Lifetime  |   Seed    |  Vel XYZ  |
};

PS_OUTPUT main(PS_INPUT input) {
    PS_OUTPUT output;
    float packedDir = packVec3(normalize(input.v_Direction));
    output.o_ParticleData0 = float4(input.v_Position, packedDir);
    output.o_ParticleData1 = float4(0.0, input.v_Particle.y, input.v_Particle.z, packedDir);
    return output;
}
