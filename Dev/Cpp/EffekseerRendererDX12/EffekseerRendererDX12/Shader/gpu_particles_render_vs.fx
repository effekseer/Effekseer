struct ParticleData
{
    uint FlagBits;
    uint Seed;
    float LifeAge;
    uint InheritColor;
    uint Color;
    uint Direction;
    uint2 Velocity;
    column_major float4x3 Transform;
};

struct VS_Input
{
    float3 Pos;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float2 UV;
    float4 Color;
    uint InstanceID;
    uint VertexID;
};

struct VS_Output
{
    float4 Pos;
    float2 UV;
    float4 Color;
    float3 WorldN;
    float3 WorldB;
    float3 WorldT;
};

struct ParameterData
{
    int EmitCount;
    int EmitPerFrame;
    float EmitOffset;
    uint Padding0;
    float2 LifeTime;
    uint EmitShapeType;
    uint EmitRotationApplied;
    float4 EmitShapeData[2];
    float3 Direction;
    float Spread;
    float2 InitialSpeed;
    float2 Damping;
    float4 AngularOffset[2];
    float4 AngularVelocity[2];
    float4 ScaleData1[2];
    float4 ScaleData2[2];
    float3 ScaleEasing;
    uint ScaleFlags;
    float3 Gravity;
    uint Padding2;
    float3 VortexCenter;
    float VortexRotation;
    float3 VortexAxis;
    float VortexAttraction;
    float TurbulencePower;
    uint TurbulenceSeed;
    float TurbulenceScale;
    float TurbulenceOctave;
    uint RenderState;
    uint ShapeType;
    uint ShapeData;
    float ShapeSize;
    float Emissive;
    float FadeIn;
    float FadeOut;
    uint MaterialType;
    uint4 ColorData;
    float3 ColorEasing;
    uint ColorFlags;
};

struct RenderConstants
{
    float3 CameraPos;
    uint CoordinateReversed;
    float3 CameraFront;
    float Reserved1;
    float3 LightDir;
    float Reserved2;
    float4 LightColor;
    float4 LightAmbient;
    column_major float4x4 ProjMat;
    column_major float4x4 CameraMat;
    column_major float4x3 BillboardMat;
    column_major float4x3 YAxisFixedMat;
};

struct EmitterData
{
    uint FlagBits;
    uint Seed;
    uint ParticleHead;
    uint ParticleSize;
    uint TrailHead;
    uint TrailSize;
    uint TrailPhase;
    uint NextEmitCount;
    uint TotalEmitCount;
    uint EmitPointCount;
    float TimeCount;
    float TimeStopped;
    uint Reserved0;
    uint Reserved1;
    float DeltaTime;
    uint Color;
    column_major float4x3 Transform;
};

struct TrailData
{
    float3 Position;
    uint Direction;
};

cbuffer cb1 : register(b1)
{
    ParameterData _121_paramData : packoffset(c0);
};

cbuffer cb0 : register(b0)
{
    RenderConstants _136_constants : packoffset(c0);
};

cbuffer cb2 : register(b2)
{
    EmitterData _265_emitter : packoffset(c0);
};

ByteAddressBuffer Trails : register(t1);
ByteAddressBuffer Particles : register(t0);

static float4 gl_Position;
static int gl_VertexIndex;
static int gl_InstanceIndex;
static float3 input_Pos;
static float3 input_Normal;
static float3 input_Binormal;
static float3 input_Tangent;
static float2 input_UV;
static float4 input_Color;
static float2 _entryPointOutput_UV;
static float4 _entryPointOutput_Color;
static float3 _entryPointOutput_WorldN;
static float3 _entryPointOutput_WorldB;
static float3 _entryPointOutput_WorldT;

struct SPIRV_Cross_Input
{
    float3 input_Pos : TEXCOORD0;
    float3 input_Normal : TEXCOORD1;
    float3 input_Binormal : TEXCOORD2;
    float3 input_Tangent : TEXCOORD3;
    float2 input_UV : TEXCOORD4;
    float4 input_Color : TEXCOORD5;
    uint gl_VertexIndex : SV_VertexID;
    uint gl_InstanceIndex : SV_InstanceID;
};

struct SPIRV_Cross_Output
{
    float2 _entryPointOutput_UV : TEXCOORD0;
    float4 _entryPointOutput_Color : TEXCOORD1;
    float3 _entryPointOutput_WorldN : TEXCOORD2;
    float3 _entryPointOutput_WorldB : TEXCOORD3;
    float3 _entryPointOutput_WorldT : TEXCOORD4;
    float4 gl_Position : SV_Position;
};

float3 UnpackNormalizedFloat3(uint bits)
{
    float3 v = float3(uint3(bits, bits >> uint(10), bits >> uint(20)) & uint3(1023u, 1023u, 1023u));
    return ((v / 1023.0f.xxx) * 2.0f) - 1.0f.xxx;
}

void transformSprite(ParticleData particle, inout float3 position)
{
    position = mul(float4(position, 0.0f), particle.Transform);
    if (_121_paramData.ShapeData == 0u)
    {
        position = mul(float4(position, 0.0f), _136_constants.BillboardMat);
    }
    else
    {
        if (_121_paramData.ShapeData == 1u)
        {
            uint param = particle.Direction;
            float3 U = normalize(UnpackNormalizedFloat3(param));
            float3 F = _136_constants.CameraFront;
            float3 R = normalize(cross(U, F));
            F = normalize(cross(R, U));
            position = mul(position, float3x3(float3(R), float3(U), float3(F)));
        }
        else
        {
            if (_121_paramData.ShapeData == 2u)
            {
                position = mul(float4(position, 0.0f), _136_constants.YAxisFixedMat);
            }
        }
    }
    position += particle.Transform[3];
}

void transformModel(ParticleData particle, inout float3 position)
{
    if (_136_constants.CoordinateReversed != 0u)
    {
        position.z = -position.z;
    }
    position = mul(float4(position, 1.0f), particle.Transform);
}

void transformTrail(ParticleData particle, inout float3 position, inout float2 uv, uint instanceID, uint vertexID)
{
    uint updateCount = (particle.FlagBits >> uint(1)) & 255u;
    uint trailLength = min(_121_paramData.ShapeData, updateCount);
    uint segmentID = min((vertexID / 2u), trailLength);
    float3 trailPosition;
    float3 trailDirection;
    if (segmentID == 0u)
    {
        trailPosition = particle.Transform[3];
        uint param = particle.Direction;
        trailDirection = normalize(UnpackNormalizedFloat3(param));
    }
    else
    {
        uint trailID = _265_emitter.TrailHead + (instanceID * _121_paramData.ShapeData);
        trailID += ((((_121_paramData.ShapeData + _265_emitter.TrailPhase) - segmentID) + 1u) % _121_paramData.ShapeData);
        TrailData _299;
        _299.Position = asfloat(Trails.Load3(trailID * 16 + 0));
        _299.Direction = Trails.Load(trailID * 16 + 12);
        TrailData trail;
        trail.Position = _299.Position;
        trail.Direction = _299.Direction;
        trailPosition = trail.Position;
        uint param_1 = trail.Direction;
        trailDirection = normalize(UnpackNormalizedFloat3(param_1));
        uv.y = float(segmentID) / float(trailLength);
    }
    float3 trailTangent = normalize(cross(_136_constants.CameraFront, trailDirection));
    position = (trailTangent * position.x) * _121_paramData.ShapeSize;
    position += trailPosition;
}

float4 UnpackColor(uint color32)
{
    return float4(float(color32 & 255u), float((color32 >> uint(8)) & 255u), float((color32 >> uint(16)) & 255u), float((color32 >> uint(24)) & 255u)) / 255.0f.xxxx;
}

VS_Output _main(VS_Input _input)
{
    uint index = _265_emitter.ParticleHead + _input.InstanceID;
    ParticleData _349;
    _349.FlagBits = Particles.Load(index * 80 + 0);
    _349.Seed = Particles.Load(index * 80 + 4);
    _349.LifeAge = asfloat(Particles.Load(index * 80 + 8));
    _349.InheritColor = Particles.Load(index * 80 + 12);
    _349.Color = Particles.Load(index * 80 + 16);
    _349.Direction = Particles.Load(index * 80 + 20);
    _349.Velocity = Particles.Load2(index * 80 + 24);
    _349.Transform = asfloat(uint4x3(Particles.Load(index * 80 + 32), Particles.Load(index * 80 + 48), Particles.Load(index * 80 + 64), Particles.Load(index * 80 + 36), Particles.Load(index * 80 + 52), Particles.Load(index * 80 + 68), Particles.Load(index * 80 + 40), Particles.Load(index * 80 + 56), Particles.Load(index * 80 + 72), Particles.Load(index * 80 + 44), Particles.Load(index * 80 + 60), Particles.Load(index * 80 + 76)));
    ParticleData particle;
    particle.FlagBits = _349.FlagBits;
    particle.Seed = _349.Seed;
    particle.LifeAge = _349.LifeAge;
    particle.InheritColor = _349.InheritColor;
    particle.Color = _349.Color;
    particle.Direction = _349.Direction;
    particle.Velocity = _349.Velocity;
    particle.Transform = _349.Transform;
    VS_Output _output;
    if ((particle.FlagBits & 1u) != 0u)
    {
        float3 position = _input.Pos;
        float2 uv = _input.UV;
        float4 color = _input.Color;
        if (_121_paramData.ShapeType == 0u)
        {
            ParticleData param = particle;
            float3 param_1 = position;
            transformSprite(param, param_1);
            position = param_1;
        }
        else
        {
            if (_121_paramData.ShapeType == 1u)
            {
                ParticleData param_2 = particle;
                float3 param_3 = position;
                transformModel(param_2, param_3);
                position = param_3;
            }
            else
            {
                if (_121_paramData.ShapeType == 2u)
                {
                    ParticleData param_4 = particle;
                    float3 param_5 = position;
                    float2 param_6 = uv;
                    uint param_7 = _input.InstanceID;
                    uint param_8 = _input.VertexID;
                    transformTrail(param_4, param_5, param_6, param_7, param_8);
                    position = param_5;
                    uv = param_6;
                }
            }
        }
        uint param_9 = particle.Color;
        color *= UnpackColor(param_9);
        float4 _432 = color;
        float3 _434 = _432.xyz * _121_paramData.Emissive;
        color.x = _434.x;
        color.y = _434.y;
        color.z = _434.z;
        _output.Pos = mul(_136_constants.ProjMat, mul(_136_constants.CameraMat, float4(position, 1.0f)));
        _output.UV = uv;
        _output.Color = color;
        if (_121_paramData.MaterialType == 1u)
        {
            _output.WorldN = mul(float4(_input.Normal, 0.0f), particle.Transform);
            _output.WorldB = mul(float4(_input.Binormal, 0.0f), particle.Transform);
            _output.WorldT = mul(float4(_input.Tangent, 0.0f), particle.Transform);
        }
    }
    else
    {
        _output.Pos = 0.0f.xxxx;
        _output.UV = 0.0f.xx;
        _output.Color = 0.0f.xxxx;
        if (_121_paramData.MaterialType == 1u)
        {
            _output.WorldN = 0.0f.xxx;
            _output.WorldB = 0.0f.xxx;
            _output.WorldT = 0.0f.xxx;
        }
    }
    return _output;
}

void vert_main()
{
    VS_Input _input;
    _input.Pos = input_Pos;
    _input.Normal = input_Normal;
    _input.Binormal = input_Binormal;
    _input.Tangent = input_Tangent;
    _input.UV = input_UV;
    _input.Color = input_Color;
    _input.InstanceID = uint(gl_InstanceIndex);
    _input.VertexID = uint(gl_VertexIndex);
    VS_Output flattenTemp = _main(_input);
    gl_Position = flattenTemp.Pos;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_WorldN = flattenTemp.WorldN;
    _entryPointOutput_WorldB = flattenTemp.WorldB;
    _entryPointOutput_WorldT = flattenTemp.WorldT;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_VertexIndex = int(stage_input.gl_VertexIndex);
    gl_InstanceIndex = int(stage_input.gl_InstanceIndex);
    input_Pos = stage_input.input_Pos;
    input_Normal = stage_input.input_Normal;
    input_Binormal = stage_input.input_Binormal;
    input_Tangent = stage_input.input_Tangent;
    input_UV = stage_input.input_UV;
    input_Color = stage_input.input_Color;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output._entryPointOutput_UV = _entryPointOutput_UV;
    stage_output._entryPointOutput_Color = _entryPointOutput_Color;
    stage_output._entryPointOutput_WorldN = _entryPointOutput_WorldN;
    stage_output._entryPointOutput_WorldB = _entryPointOutput_WorldB;
    stage_output._entryPointOutput_WorldT = _entryPointOutput_WorldT;
    return stage_output;
}
