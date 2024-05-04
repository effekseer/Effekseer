#include "gpu_particles_utils.fx"

cbuffer cb0 : register(b0)
{
    ComputeConstants constants;
};
cbuffer cb1 : register(b1)
{
    ParameterData paramData;
}
cbuffer cb2 : register(b2)
{
    EmitterData emitter;
}

StructuredBuffer<EmitPoint> EmitPoints : register(t1);
RWStructuredBuffer<ParticleData> Particles : register(u0);

[numthreads(1, 1, 1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    uint seed = emitter.Seed ^ (emitter.TotalEmitCount + dtid.x);
    float3 position = float3(0.0f, 0.0f, 0.0f);
    float3 direction = RandomSpread(seed, paramData.Direction, paramData.Spread * PI / 180.0f);
    float speed = RandomFloatRange(seed, paramData.InitialSpeed);

    if (paramData.EmitShapeType == 1) {
        float3 lineStart = paramData.EmitShapeData[0].xyz;
        float3 lineEnd = paramData.EmitShapeData[1].xyz;
        float lineWidth = paramData.EmitShapeData[1].w;
        position += lerp(lineStart, lineEnd, RandomFloat(seed));
        position += RandomDirection(seed) * lineWidth * 0.5f;
    } else if (paramData.EmitShapeType == 2) {
        float3 circleAxis = paramData.EmitShapeData[0].xyz;
        float circleInner = paramData.EmitShapeData[1].x;
        float circleOuter = paramData.EmitShapeData[1].y;
        float circleRadius = sqrt(lerp(circleInner * circleInner, circleOuter * circleOuter, RandomFloat(seed)));
        float3 circleDirection = RandomCircle(seed, circleAxis);
        position += circleDirection * circleRadius;
        if (paramData.EmitRotationApplied) {
            if (constants.CoordinateReversed) {
                circleDirection.z = -circleDirection.z;
            }
            direction = mul(direction, float3x3(cross(circleAxis, circleDirection), circleAxis, circleDirection));
        }
    } else if (paramData.EmitShapeType == 3) {
        float sphereRadius = paramData.EmitShapeData[0].x;
        float3 sphereDirection = RandomDirection(seed);
        position += sphereDirection * sphereRadius;
        if (paramData.EmitRotationApplied) {
            float3 sphereUp = float3(0.0f, 1.0f, 0.0f);
            if (constants.CoordinateReversed) {
                sphereDirection.z = -sphereDirection.z;
            }
            direction = mul(direction, float3x3(cross(sphereUp, sphereDirection), sphereUp, sphereDirection));
        }
    } else if (paramData.EmitShapeType == 4) {
        float modelSize = paramData.EmitShapeData[0].y;
        if (emitter.EmitPointCount > 0) {
            uint emitIndex = RandomUint(seed) % emitter.EmitPointCount;
            float3 emitPosition = EmitPoints[emitIndex].Position;
            if (constants.CoordinateReversed) {
                emitPosition.z = -emitPosition.z;
            }
            position += emitPosition * modelSize;
            if (paramData.EmitRotationApplied) {
                float3 emitNormal = normalize(UnpackNormalizedFloat3(EmitPoints[emitIndex].Normal));
                float3 emitTangent = normalize(UnpackNormalizedFloat3(EmitPoints[emitIndex].Tangent));
                float3 emitBinormal = normalize(cross(emitTangent, emitNormal));
                direction = mul(direction, float3x3(emitTangent, emitBinormal, emitNormal));
            }
        }
    }
    if (constants.CoordinateReversed) {
        direction.z = -direction.z;
    }
    position = mul(float4(position, 1.0f), emitter.Transform).xyz;
    direction = mul(float4(direction, 0.0f), emitter.Transform).xyz;

    uint particleID = emitter.ParticleHead + (emitter.TotalEmitCount + dtid.x) % emitter.ParticleSize;
    ParticleData particle;
    particle.FlagBits = 0x01;
    particle.Seed = seed;
    particle.LifeAge = 0.0f;
    
    if (paramData.ColorFlags == 0) {
        particle.InheritColor = 0xFFFFFFFF;
    } else {
        particle.InheritColor = emitter.Color;
    }
    particle.Color = 0xFFFFFFFF;

    particle.Transform = TRSMatrix(position, float3(0.0, 0.0, 0.0), float3(1.0, 1.0, 1.0));
    particle.Direction = PackNormalizedFloat3(direction);
    particle.Velocity = PackFloat4(float4(direction * speed, 0.0f));
    Particles[particleID] = particle;
}
