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

RWStructuredBuffer<ParticleData> Particles : register(u0);
RWStructuredBuffer<TrailData> Trails : register(u1);
Texture3D<float4> NoiseTex : register(t2);
SamplerState NoiseSamp : register(s2);
Texture3D<float4> FieldTex : register(t3);
SamplerState FieldSamp : register(s3);
Texture2D<float4> GradientTex : register(t4);
SamplerState GradientSamp : register(s4);

float3 Vortex(float rotation, float attraction, float3 center, float3 axis, float3 position, float4x3 transform) {
    
    center = transform[3] + center;
    axis = normalize(mul(float4(axis, 0.0f), transform));

    float3 localPos = position - center;
    float3 axisToPos = localPos - axis * dot(axis, localPos);
    float distance = length(axisToPos);
    if (distance < 0.0001f) {
        return float3(0.0f, 0.0f, 0.0f);
    }

    float3 radial = normalize(axisToPos);
    float3 tangent = cross(axis, radial);
    return tangent * rotation - radial * attraction;
}

[numthreads(256, 1, 1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    uint particleID = emitter.ParticleHead + dtid.x;
    ParticleData particle = Particles[particleID];
    
    if (particle.FlagBits & 0x01) {
        uint updateCount = (particle.FlagBits >> 1) & 0xFF;
        float deltaTime = emitter.DeltaTime;

        // Randomize parameters
        uint seed = particle.Seed;
        float lifeTime = RandomFloatRange(seed, paramData.LifeTime);
        float lifeRatio = particle.LifeAge / lifeTime;
        float damping = RandomFloatRange(seed, paramData.Damping) * 0.01;
        float4 angularOffset = RandomFloat4Range(seed, paramData.AngularOffset);
        float4 angularVelocity = RandomFloat4Range(seed, paramData.AngularVelocity);

        float3 position = particle.Transform[3];
        float3 lastPosition = position;
        float3 direction = normalize(UnpackNormalizedFloat3(particle.Direction));
        float3 velocity = UnpackFloat4(particle.Velocity).xyz;

        if (emitter.TrailSize > 0) {
            uint trailID = emitter.TrailHead + dtid.x * paramData.ShapeData + emitter.TrailPhase;
            TrailData trail;
            trail.Position = position;
            trail.Direction = PackNormalizedFloat3(direction);
            Trails[trailID] = trail;
        }
        
        // Increase count
        particle.FlagBits &= ~(0xFF << 1);
        particle.FlagBits |= clamp(updateCount + 1, 0, 255) << 1;

        // Aging
        particle.LifeAge += deltaTime;
        if (particle.LifeAge >= lifeTime) {
            // Clear the alive flag
            particle.FlagBits &= ~1;
        }
        
        // Gravity
        velocity += paramData.Gravity * deltaTime;
        
        // Damping
        float speed = length(velocity);
        if (speed > 0.0f) {
            float newSpeed = max(0.0f, speed - damping * deltaTime);
            velocity *= newSpeed / speed;
        }

        // Move from velocity
        position += velocity * deltaTime;

        // Vortex
        if (paramData.VortexRotation != 0.0f || paramData.VortexAttraction != 0.0f) {
            position += Vortex(paramData.VortexRotation, paramData.VortexAttraction, 
                paramData.VortexCenter, paramData.VortexAxis,
                position, emitter.Transform) * deltaTime;
        }
        // Turbulence
        if (paramData.TurbulencePower != 0.0f) {
            float4 vfTexel = NoiseTex.SampleLevel(NoiseSamp, position * paramData.TurbulenceScale + 0.5f, 0);
            position += (vfTexel.xyz * 2.0f - 1.0f) * paramData.TurbulencePower * deltaTime;
        }

        // Calc direction
        float3 diff = position - lastPosition;
        if (length(diff) > 0.0001f) {
            direction = normalize(diff);
        }

        // Rotation (Euler)
        float3 rotation = angularOffset.xyz + angularVelocity.xyz * particle.LifeAge;
        
        // Scale (XYZ+Single)
        float4 scale = float4(1.0f, 1.0f, 1.0f, 1.0f);
        uint scaleMode = paramData.ScaleFlags & 0x07;
        if (scaleMode == 0) {
            scale = RandomFloat4Range(seed, paramData.ScaleData1);
        } else if (scaleMode == 2) {
            float4 scale1 = RandomFloat4Range(seed, paramData.ScaleData1);
            float4 scale2 = RandomFloat4Range(seed, paramData.ScaleData2);
            scale = lerp(scale1, scale2, EasingSpeed(lifeRatio, paramData.ScaleEasing));
        }

        // Color
        uint colorMode = paramData.ColorFlags & 0x07;
        float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
        if (colorMode == 0) {
            color = UnpackColor(paramData.ColorData.x);
        } else if (colorMode == 1) {
            color = RandomColorRange(seed, paramData.ColorData.xy);
        } else if (colorMode == 2) {
            float4 colorStart = RandomColorRange(seed, paramData.ColorData.xy);
            float4 colorEnd = RandomColorRange(seed, paramData.ColorData.zw);
            color = lerp(colorStart, colorEnd, EasingSpeed(lifeRatio, paramData.ColorEasing));
        } else if (colorMode == 3 || colorMode == 4) {
            color = GradientTex.SampleLevel(GradientSamp, float2(lifeRatio, 0.0f), 0);
        }
        
        // HSV
        if ((paramData.ColorFlags >> 5) & 0x01) {
            color.rgb = HSV2RGB(color.rgb);
        }

        // Apply inheritation color
        uint colorInherit = (paramData.ColorFlags >> 3) & 0x03;
        if (colorInherit == 2 || colorInherit == 3) {
            color *= UnpackColor(emitter.Color);
        } else {
            color *= UnpackColor(particle.InheritColor);
        }

        // Fade-in/out
        color.a *= clamp(particle.LifeAge / paramData.FadeIn, 0.0, 1.0);
        color.a *= clamp((lifeTime - particle.LifeAge) / paramData.FadeOut, 0.0, 1.0);

        particle.Transform = TRSMatrix(position, rotation, scale.xyz * scale.w * paramData.ShapeSize);
        particle.Velocity = PackFloat4(float4(velocity, 0.0));
        particle.Direction = PackNormalizedFloat3(direction);
        particle.Color = PackColor(color);
        Particles[particleID] = particle;
    }
}
