static const char metal_model_distortion_vs[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_Input
{
    float3 Pos;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float2 UV;
    float4 Color;
    uint Index;
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
    float4x4 mCameraProj;
    float4x4 mModel_Inst[40];
    float4 fUV[40];
    float4 fModelColor[40];
    float4 fLightDirection;
    float4 fLightColor;
    float4 fLightAmbient;
    float4 mUVInversed;
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
    float3 Input_Normal [[attribute(1)]];
    float3 Input_Binormal [[attribute(2)]];
    float3 Input_Tangent [[attribute(3)]];
    float2 Input_UV [[attribute(4)]];
    float4 Input_Color [[attribute(5)]];
};

static inline __attribute__((always_inline))
VS_Output _main(VS_Input Input, constant VS_ConstantBuffer& v_31)
{
    uint index = Input.Index;
    float4x4 mModel = transpose(v_31.mModel_Inst[index]);
    float4 uv = v_31.fUV[index];
    float4 modelColor = v_31.fModelColor[index] * Input.Color;
    VS_Output Output = VS_Output{ float4(0.0), float2(0.0), float4(0.0), float4(0.0), float4(0.0), float4(0.0) };
    float4 localPos = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    float4 worldPos = localPos * mModel;
    Output.PosVS = v_31.mCameraProj * worldPos;
    Output.Color = modelColor;
    float2 outputUV = Input.UV;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = v_31.mUVInversed.x + (v_31.mUVInversed.y * outputUV.y);
    Output.UV = outputUV;
    float4 localNormal = float4(Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0);
    float4 localBinormal = float4(Input.Binormal.x, Input.Binormal.y, Input.Binormal.z, 0.0);
    float4 localTangent = float4(Input.Tangent.x, Input.Tangent.y, Input.Tangent.z, 0.0);
    float4 worldNormal = localNormal * mModel;
    float4 worldBinormal = localBinormal * mModel;
    float4 worldTangent = localTangent * mModel;
    worldNormal = normalize(worldNormal);
    worldBinormal = normalize(worldBinormal);
    worldTangent = normalize(worldTangent);
    Output.ProjBinormal = v_31.mCameraProj * (worldPos + worldBinormal);
    Output.ProjTangent = v_31.mCameraProj * (worldPos + worldTangent);
    Output.PosP = Output.PosVS;
    return Output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_31 [[buffer(0)]], uint gl_InstanceIndex [[instance_id]])
{
    main0_out out = {};
    VS_Input Input;
    Input.Pos = in.Input_Pos;
    Input.Normal = in.Input_Normal;
    Input.Binormal = in.Input_Binormal;
    Input.Tangent = in.Input_Tangent;
    Input.UV = in.Input_UV;
    Input.Color = in.Input_Color;
    Input.Index = gl_InstanceIndex;
    VS_Output flattenTemp = _main(Input, v_31);
    out.gl_Position = flattenTemp.PosVS;
    out._entryPointOutput_UV = flattenTemp.UV;
    out._entryPointOutput_ProjBinormal = flattenTemp.ProjBinormal;
    out._entryPointOutput_ProjTangent = flattenTemp.ProjTangent;
    out._entryPointOutput_PosP = flattenTemp.PosP;
    out._entryPointOutput_Color = flattenTemp.Color;
    return out;
}

)";

