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
    float4 PosVS;
    float2 UV;
    float4 ProjBinormal;
    float4 ProjTangent;
    float4 PosP;
    float4 Color;
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
    float2 _entryPointOutput_UV [[user(locn0)]];
    float4 _entryPointOutput_ProjBinormal [[user(locn1)]];
    float4 _entryPointOutput_ProjTangent [[user(locn2)]];
    float4 _entryPointOutput_PosP [[user(locn3)]];
    float4 _entryPointOutput_Color [[user(locn4)]];
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
VS_Output _main(VS_Input Input, constant VS_ConstantBuffer& v_60)
{
    VS_Output Output = VS_Output{ float4(0.0), float2(0.0), float4(0.0), float4(0.0), float4(0.0), float4(0.0) };
    float4 pos4 = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    float3 worldNormal = (Input.Normal.xyz - float3(0.5)) * 2.0;
    float3 worldTangent = (Input.Tangent.xyz - float3(0.5)) * 2.0;
    float3 worldBinormal = cross(worldNormal, worldTangent);
    float4 cameraPos = v_60.mCamera * pos4;
    Output.PosVS = v_60.mProj * cameraPos;
    Output.PosP = Output.PosVS;
    float4 localTangent = pos4;
    float4 localBinormal = pos4;
    float3 _82 = localTangent.xyz + worldTangent;
    localTangent = float4(_82.x, _82.y, _82.z, localTangent.w);
    float3 _88 = localBinormal.xyz + worldBinormal;
    localBinormal = float4(_88.x, _88.y, _88.z, localBinormal.w);
    Output.ProjTangent = v_60.mProj * (v_60.mCamera * localTangent);
    Output.ProjBinormal = v_60.mProj * (v_60.mCamera * localBinormal);
    Output.Color = Input.Color;
    Output.UV = Input.UV1;
    Output.UV.y = v_60.mUVInversed.x + (v_60.mUVInversed.y * Input.UV1.y);
    return Output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_60 [[buffer(0)]])
{
    main0_out out = {};
    VS_Input Input;
    Input.Pos = in.Input_Pos;
    Input.Color = in.Input_Color;
    Input.Normal = in.Input_Normal;
    Input.Tangent = in.Input_Tangent;
    Input.UV1 = in.Input_UV1;
    Input.UV2 = in.Input_UV2;
    VS_Output flattenTemp = _main(Input, v_60);
    out.gl_Position = flattenTemp.PosVS;
    out._entryPointOutput_UV = flattenTemp.UV;
    out._entryPointOutput_ProjBinormal = flattenTemp.ProjBinormal;
    out._entryPointOutput_ProjTangent = flattenTemp.ProjTangent;
    out._entryPointOutput_PosP = flattenTemp.PosP;
    out._entryPointOutput_Color = flattenTemp.Color;
    return out;
}

