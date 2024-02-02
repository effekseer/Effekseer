
struct ParameterData
{
    int EmitCount;
    int EmitPerFrame;
    float EmitOffset;
    uint Padding0;

    float2 LifeTime;
    uint EmitShapeType;
    uint Padding1;
    
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
    float4x3 Transform;
};

struct ParticleData
{
    uint FlagBits;
    uint Seed;
    float LifeAge;
    uint InheritColor;
    uint2 DirectionSpeed;
    uint Color;
    uint Padding;
    float4x3 Transform;
};

struct TrailData
{
    float3 Position;
    uint Direction;
};

struct EmitPoint
{
    float3 Position;
    uint Normal;
    uint Binormal;
    uint Tangent;
    uint UV;
    uint VColor;
};

struct DrawConstants
{
    float3 CameraPos;
    float Reserved0;
    float3 CameraFront;
    float Reserved1;
    
    float3 LightDir;
    float Reserved2;
    float4 LightColor;
    float4 LightAmbient;
    
    float4x4 ProjMat;
    float4x4 CameraMat;
    float4x3 BillboardMat;
    float4x3 YAxisFixedMat;
};
