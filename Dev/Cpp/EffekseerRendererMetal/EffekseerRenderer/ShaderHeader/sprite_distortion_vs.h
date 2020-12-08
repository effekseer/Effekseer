static const char metal_sprite_distortion_vs[] = R"(mtlcode
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
    float4 Color;
    float2 UV;
    float4 PosP;
    float4 PosU;
    float4 PosR;
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
    float4 _entryPointOutput_Color [[user(locn0)]];
    float2 _entryPointOutput_UV [[user(locn1)]];
    float4 _entryPointOutput_PosP [[user(locn2)]];
    float4 _entryPointOutput_PosU [[user(locn3)]];
    float4 _entryPointOutput_PosR [[user(locn4)]];
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
VS_Output _main(VS_Input Input, constant VS_ConstantBuffer& v_88)
{
    VS_Output Output = VS_Output{ float4(0.0), float4(0.0), float2(0.0), float4(0.0), float4(0.0), float4(0.0) };
    float4 pos4 = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    float3 worldNormal = (Input.Normal.xyz - float3(0.5)) * 2.0;
    float3 worldTangent = (Input.Tangent.xyz - float3(0.5)) * 2.0;
    float3 worldBinormal = cross(worldNormal, worldTangent);
    float4 localBinormal = float4(Input.Pos.x + worldBinormal.x, Input.Pos.y + worldBinormal.y, Input.Pos.z + worldBinormal.z, 1.0);
    float4 localTangent = float4(Input.Pos.x + worldTangent.x, Input.Pos.y + worldTangent.y, Input.Pos.z + worldTangent.z, 1.0);
    localBinormal = v_88.mCamera * localBinormal;
    localTangent = v_88.mCamera * localTangent;
    float4 cameraPos = v_88.mCamera * pos4;
    cameraPos /= float4(cameraPos.w);
    localBinormal /= float4(localBinormal.w);
    localTangent /= float4(localTangent.w);
    localBinormal = cameraPos + normalize(localBinormal - cameraPos);
    localTangent = cameraPos + normalize(localTangent - cameraPos);
    Output.PosVS = v_88.mProj * cameraPos;
    Output.PosP = Output.PosVS;
    Output.PosU = v_88.mProj * localBinormal;
    Output.PosR = v_88.mProj * localTangent;
    Output.PosU /= float4(Output.PosU.w);
    Output.PosR /= float4(Output.PosR.w);
    Output.PosP /= float4(Output.PosP.w);
    Output.Color = Input.Color;
    Output.UV = Input.UV1;
    Output.UV.y = v_88.mUVInversed.x + (v_88.mUVInversed.y * Input.UV1.y);
    return Output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_88 [[buffer(0)]])
{
    main0_out out = {};
    VS_Input Input;
    Input.Pos = in.Input_Pos;
    Input.Color = in.Input_Color;
    Input.Normal = in.Input_Normal;
    Input.Tangent = in.Input_Tangent;
    Input.UV1 = in.Input_UV1;
    Input.UV2 = in.Input_UV2;
    VS_Output flattenTemp = _main(Input, v_88);
    out.gl_Position = flattenTemp.PosVS;
    out._entryPointOutput_Color = flattenTemp.Color;
    out._entryPointOutput_UV = flattenTemp.UV;
    out._entryPointOutput_PosP = flattenTemp.PosP;
    out._entryPointOutput_PosU = flattenTemp.PosU;
    out._entryPointOutput_PosR = flattenTemp.PosR;
    return out;
}

)";

