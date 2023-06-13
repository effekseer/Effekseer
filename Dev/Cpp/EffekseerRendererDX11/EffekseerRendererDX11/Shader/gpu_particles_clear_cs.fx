#include "gpu_particles_utils.fx"

cbuffer cb0 : register(b0)
{
    ParameterData paramData;
}
cbuffer cb1 : register(b1)
{
    EmitterData emitter;
}

RWStructuredBuffer<ParticleData> Particles : register(u0);

[numthreads(256, 1, 1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    uint particleID = emitter.ParticleHead + dtid.x;
    
    ParticleData particle;
    particle.FlagBits = 0;
    particle.Seed = 0;
    particle.LifeAge = 0.0f;
    particle.InheritColor = 0;
    particle.Color = 0;
    particle.Padding = 0;
    particle.DirectionSpeed = PackFloat4(float4(0.0f, 0.0f, 0.0f, 0.0f));
    particle.Transform = float4x3(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    
    Particles[particleID] = particle;
}
