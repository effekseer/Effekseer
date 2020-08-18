static const char metal_sprite_lit_vs[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_Input
{
    float3 Pos;
    float4 Color;
    float4 Normal;
    float4 Tangent;
    float2 UV1;
    float2 UV2;
};

struct VS_Output
{
    float4 Position;
    float4 VColor;
    float2 UV1;
    float2 UV2;
    float3 WorldP;
    float3 WorldN;
    float3 WorldT;
    float3 WorldB;
    float2 ScreenUV;
};

struct VS_ConstantBuffer
{
    float4x4 mCamera;
    float4x4 mProj;
    float4 mUVInversed;
    float4 mflipbookParameter;
};

struct main0_out
{
    float4 _entryPointOutput_VColor [[user(locn0)]];
    float2 _entryPointOutput_UV1 [[user(locn1)]];
    float2 _entryPointOutput_UV2 [[user(locn2)]];
    float3 _entryPointOutput_WorldP [[user(locn3)]];
    float3 _entryPointOutput_WorldN [[user(locn4)]];
    float3 _entryPointOutput_WorldT [[user(locn5)]];
    float3 _entryPointOutput_WorldB [[user(locn6)]];
    float2 _entryPointOutput_ScreenUV [[user(locn7)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float3 Input_Pos [[attribute(0)]];
    float4 Input_Color [[attribute(1)]];
    float4 Input_Normal [[attribute(2)]];
    float4 Input_Tangent [[attribute(3)]];
    float2 Input_UV1 [[attribute(4)]];
    float2 Input_UV2 [[attribute(5)]];
};

static inline __attribute__((always_inline))
VS_Output _main(VS_Input Input, constant VS_ConstantBuffer& v_62)
{
    VS_Output Output = VS_Output{ float4(0.0), float4(0.0), float2(0.0), float2(0.0), float3(0.0), float3(0.0), float3(0.0), float3(0.0), float2(0.0) };
    float3 worldPos = Input.Pos;
    float3 worldNormal = (float3(Input.Normal.xyz) - float3(0.5)) * 2.0;
    float3 worldTangent = (float3(Input.Tangent.xyz) - float3(0.5)) * 2.0;
    float3 worldBinormal = cross(worldNormal, worldTangent);
    float2 uv1 = Input.UV1;
    float2 uv2 = Input.UV1;
    uv1.y = v_62.mUVInversed.x + (v_62.mUVInversed.y * uv1.y);
    uv2.y = v_62.mUVInversed.x + (v_62.mUVInversed.y * uv2.y);
    Output.WorldN = worldNormal;
    Output.WorldB = worldBinormal;
    Output.WorldT = worldTangent;
    float3 pixelNormalDir = float3(0.5, 0.5, 1.0);
    float4 cameraPos = v_62.mCamera * float4(worldPos, 1.0);
    cameraPos /= float4(cameraPos.w);
    Output.Position = v_62.mProj * cameraPos;
    Output.WorldP = worldPos;
    Output.VColor = Input.Color;
    Output.UV1 = uv1;
    Output.UV2 = uv2;
    Output.ScreenUV = Output.Position.xy / float2(Output.Position.w);
    Output.ScreenUV = float2(Output.ScreenUV.x + 1.0, 1.0 - Output.ScreenUV.y) * 0.5;
    return Output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_62 [[buffer(0)]])
{
    main0_out out = {};
    VS_Input Input;
    Input.Pos = in.Input_Pos;
    Input.Color = in.Input_Color;
    Input.Normal = in.Input_Normal;
    Input.Tangent = in.Input_Tangent;
    Input.UV1 = in.Input_UV1;
    Input.UV2 = in.Input_UV2;
    VS_Output flattenTemp = _main(Input, v_62);
    out.gl_Position = flattenTemp.Position;
    out._entryPointOutput_VColor = flattenTemp.VColor;
    out._entryPointOutput_UV1 = flattenTemp.UV1;
    out._entryPointOutput_UV2 = flattenTemp.UV2;
    out._entryPointOutput_WorldP = flattenTemp.WorldP;
    out._entryPointOutput_WorldN = flattenTemp.WorldN;
    out._entryPointOutput_WorldT = flattenTemp.WorldT;
    out._entryPointOutput_WorldB = flattenTemp.WorldB;
    out._entryPointOutput_ScreenUV = flattenTemp.ScreenUV;
    return out;
}

)";

