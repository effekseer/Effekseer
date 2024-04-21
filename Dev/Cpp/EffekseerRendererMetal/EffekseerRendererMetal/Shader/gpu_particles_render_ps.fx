#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
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
    packed_float3 Direction;
    float Spread;
    float2 InitialSpeed;
    float2 Damping;
    float4 AngularOffset[2];
    float4 AngularVelocity[2];
    float4 ScaleData1[2];
    float4 ScaleData2[2];
    packed_float3 ScaleEasing;
    uint ScaleFlags;
    packed_float3 Gravity;
    uint Padding2;
    packed_float3 VortexCenter;
    float VortexRotation;
    packed_float3 VortexAxis;
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
    packed_float3 ColorEasing;
    uint ColorFlags;
};

struct cb1
{
    ParameterData paramData;
};

struct RenderConstants
{
    packed_float3 CameraPos;
    uint CoordinateReversed;
    packed_float3 CameraFront;
    float Reserved1;
    packed_float3 LightDir;
    float Reserved2;
    float4 LightColor;
    float4 LightAmbient;
    float4x4 ProjMat;
    float4x4 CameraMat;
    float3x4 BillboardMat;
    float3x4 YAxisFixedMat;
};

struct cb0
{
    RenderConstants constants;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float2 input_UV [[user(locn0)]];
    float4 input_Color [[user(locn1)]];
    float3 input_WorldN [[user(locn2)]];
    float3 input_WorldB [[user(locn3)]];
    float3 input_WorldT [[user(locn4)]];
};

static inline __attribute__((always_inline))
float4 _main(PS_Input _input, texture2d<float> ColorTex, sampler ColorSamp, constant cb1& _45, texture2d<float> NormalTex, sampler NormalSamp, constant cb0& _103)
{
    float4 color = _input.Color * ColorTex.sample(ColorSamp, _input.UV);
    if (_45.paramData.MaterialType == 1u)
    {
        float3 texNormal = (NormalTex.sample(NormalSamp, _input.UV).xyz * 2.0) - float3(1.0);
        float3 normal = fast::normalize(float3x3(float3(_input.WorldT), float3(_input.WorldB), float3(_input.WorldN)) * texNormal);
        float diffuse = fast::max(dot(float3(_103.constants.LightDir), normal), 0.0);
        float4 _122 = color;
        float3 _124 = _122.xyz * ((_103.constants.LightColor.xyz * diffuse) + _103.constants.LightAmbient.xyz);
        color.x = _124.x;
        color.y = _124.y;
        color.z = _124.z;
    }
    return color;
}

fragment main0_out main0(main0_in in [[stage_in]], constant cb0& _103 [[buffer(0)]], constant cb1& _45 [[buffer(1)]], texture2d<float> ColorTex [[texture(2)]], texture2d<float> NormalTex [[texture(3)]], sampler ColorSamp [[sampler(2)]], sampler NormalSamp [[sampler(3)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input _input;
    _input.Pos = gl_FragCoord;
    _input.UV = in.input_UV;
    _input.Color = in.input_Color;
    _input.WorldN = in.input_WorldN;
    _input.WorldB = in.input_WorldB;
    _input.WorldT = in.input_WorldT;
    out._entryPointOutput = _main(_input, ColorTex, ColorSamp, _45, NormalTex, NormalSamp, _103);
    return out;
}

