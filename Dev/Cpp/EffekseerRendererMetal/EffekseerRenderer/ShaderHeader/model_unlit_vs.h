static const char metal_model_unlit_vs[] = R"(mtlcode
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
    float4 Color;
    float2 UV;
    float4 PosP;
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
    float4 _entryPointOutput_Color [[user(locn0)]];
    float2 _entryPointOutput_UV [[user(locn1)]];
    float4 _entryPointOutput_PosP [[user(locn2)]];
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
    VS_Output Output = VS_Output{ float4(0.0), float4(0.0), float2(0.0), float4(0.0) };
    float4 localPos = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    float4 worldPos = localPos * mModel;
    Output.PosVS = v_31.mCameraProj * worldPos;
    Output.Color = modelColor;
    float2 outputUV = Input.UV;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = v_31.mUVInversed.x + (v_31.mUVInversed.y * outputUV.y);
    Output.UV = outputUV;
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
    out._entryPointOutput_Color = flattenTemp.Color;
    out._entryPointOutput_UV = flattenTemp.UV;
    out._entryPointOutput_PosP = flattenTemp.PosP;
    return out;
}

)";

