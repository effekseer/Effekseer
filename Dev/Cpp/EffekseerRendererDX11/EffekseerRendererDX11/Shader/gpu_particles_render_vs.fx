#include "gpu_particles_utils.fx"

cbuffer cb0 : register(b0)
{
    RenderConstants constants;
};
cbuffer cb1 : register(b1)
{
    ParameterData paramData;
};
cbuffer cb2 : register(b2)
{
    EmitterData emitter;
};

struct VS_Input
{
	float3 Pos : POSITION0;
	float3 Normal : NORMAL0;
	float3 Binormal : NORMAL1;
	float3 Tangent : NORMAL2;
    float2 UV : TEXCOORD0;
    float4 Color : NORMAL3;
    uint InstanceID : SV_InstanceID;
    uint VertexID : SV_VertexID;
};

struct VS_Output
{
	float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0;
	float3 WorldN : TEXCOORD1;
	float3 WorldB : TEXCOORD2;
	float3 WorldT : TEXCOORD3;
};

StructuredBuffer<ParticleData> Particles : register(t0);
StructuredBuffer<TrailData> Trails : register(t1);

void transformSprite(ParticleData particle, inout float3 position)
{
    // Rotation and Scale Transform
    position = mul(float4(position, 0.0f), particle.Transform);

    // Billboard transform
    if (paramData.ShapeData == 0) {
        // RotatedBillboard
        position = mul(float4(position, 0.0f), constants.BillboardMat);
    }
    else if (paramData.ShapeData == 1) {
        // DirectionalBillboard
        float3 R, U, F;
        U = normalize(UnpackNormalizedFloat3(particle.Direction));
        F = constants.CameraFront;
        R = normalize(cross(U, F));
        F = normalize(cross(R, U));
        position = mul(position, float3x3(R, U, F));
    }
    else if (paramData.ShapeData == 2) {
        // YAxisFixed
        position = mul(float4(position, 0.0f), constants.YAxisFixedMat);
    }

    // Position transform
    position = position + particle.Transform[3];
}

void transformModel(ParticleData particle, inout float3 position)
{
    // Position and Rotation and Scale Transform
    if (constants.CoordinateReversed) {
        position.z = -position.z;
    }
    position = mul(float4(position, 1.0f), particle.Transform).xyz;
}

void transformTrail(ParticleData particle, inout float3 position, inout float2 uv, uint instanceID, uint vertexID)
{
    // TrailData Transform
    uint updateCount = (particle.FlagBits >> 1) & 0xFF;
    uint trailLength = min(paramData.ShapeData, updateCount);
    float3 trailPosition;
    float3 trailDirection;

    uint segmentID = min(vertexID / 2, trailLength);
    if (segmentID == 0) {
        trailPosition = particle.Transform[3];
        trailDirection = normalize(UnpackNormalizedFloat3(particle.Direction));
    }
    else {
        uint trailID = emitter.TrailHead + instanceID * paramData.ShapeData;
        trailID += (paramData.ShapeData + emitter.TrailPhase - segmentID + 1) % paramData.ShapeData;
        TrailData trail = Trails[trailID];
        trailPosition = trail.Position;
        trailDirection = normalize(UnpackNormalizedFloat3(trail.Direction));
        uv.y = float(segmentID) / float(trailLength);
    }

    float3 trailTangent = normalize(cross(constants.CameraFront, trailDirection));
    position = trailTangent * position.x * paramData.ShapeSize;
    position += trailPosition;
}

VS_Output main(const VS_Input input)
{
    VS_Output output;
    
    uint index = emitter.ParticleHead + input.InstanceID;
    ParticleData particle = Particles[index];
    if (particle.FlagBits & 0x01) {
        float3 position = input.Pos;
        float2 uv = input.UV;
        float4 color = input.Color;

        if (paramData.ShapeType == 0) {
            transformSprite(particle, position);
        } else if (paramData.ShapeType == 1) {
            transformModel(particle, position);
        } else if (paramData.ShapeType == 2) {
            transformTrail(particle, position, uv, input.InstanceID, input.VertexID);
        }

        color *= UnpackColor(particle.Color);
        color.rgb *= paramData.Emissive;
        
        output.Pos = mul(constants.ProjMat, mul(constants.CameraMat, float4(position, 1.0f)));
        output.UV = uv;
        output.Color = color;

        if (paramData.MaterialType == 1) {
            output.WorldN = mul(float4(input.Normal, 0.0f), particle.Transform).xyz;
            output.WorldB = mul(float4(input.Binormal, 0.0f), particle.Transform).xyz;
            output.WorldT = mul(float4(input.Tangent, 0.0f), particle.Transform).xyz;
        }
    }
    else {
        output.Pos = float4(0.0f, 0.0f, 0.0f, 0.0f);
        output.UV = float2(0.0f, 0.0f);
        output.Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
        
        if (paramData.MaterialType == 1) {
            output.WorldN = float3(0.0f, 0.0f, 0.0f);
            output.WorldB = float3(0.0f, 0.0f, 0.0f);
            output.WorldT = float3(0.0f, 0.0f, 0.0f);
        }
    }

    return output;
}
