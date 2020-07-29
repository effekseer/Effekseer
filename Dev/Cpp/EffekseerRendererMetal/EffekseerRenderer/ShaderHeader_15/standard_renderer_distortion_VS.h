static const char metal_standard_renderer_distortion_VS[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_Input
{
    float3 Pos;
    float4 Color;
    float2 UV;
    float3 Binormal;
    float3 Tangent;
};

struct VS_Output
{
    float4 Position;
    float4 Color;
    float2 UV;
    float4 Pos;
    float4 PosU;
    float4 PosR;
};

struct VS_ConstantBuffer
{
    float4x4 mCamera;
    float4x4 mProj;
    float4 mUVInversed;
};

struct main0_out
{
    float4 _entryPointOutput_Color [[user(locn0)]];
    float2 _entryPointOutput_UV [[user(locn1)]];
    float4 _entryPointOutput_Pos [[user(locn2)]];
    float4 _entryPointOutput_PosU [[user(locn3)]];
    float4 _entryPointOutput_PosR [[user(locn4)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float3 Input_Pos [[attribute(0)]];
    float4 Input_Color [[attribute(1)]];
    float2 Input_UV [[attribute(2)]];
    float3 Input_Binormal [[attribute(3)]];
    float3 Input_Tangent [[attribute(4)]];
};

static inline __attribute__((always_inline))
VS_Output _main(VS_Input Input, constant VS_ConstantBuffer& v_63)
{
    VS_Output Output = VS_Output{ float4(0.0), float4(0.0), float2(0.0), float4(0.0), float4(0.0), float4(0.0) };
    float4 pos4 = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    float4 localBinormal = float4(Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0);
    float4 localTangent = float4(Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0);
    localBinormal = v_63.mCamera * localBinormal;
    localTangent = v_63.mCamera * localTangent;
    float4 cameraPos = v_63.mCamera * pos4;
    cameraPos /= float4(cameraPos.w);
    localBinormal /= float4(localBinormal.w);
    localTangent /= float4(localTangent.w);
    localBinormal = cameraPos + normalize(localBinormal - cameraPos);
    localTangent = cameraPos + normalize(localTangent - cameraPos);
    Output.Position = v_63.mProj * cameraPos;
    Output.Pos = Output.Position;
    Output.PosU = v_63.mProj * localBinormal;
    Output.PosR = v_63.mProj * localTangent;
    Output.PosU /= float4(Output.PosU.w);
    Output.PosR /= float4(Output.PosR.w);
    Output.Pos /= float4(Output.Pos.w);
    Output.Color = Input.Color;
    Output.UV = Input.UV;
    Output.UV.y = v_63.mUVInversed.x + (v_63.mUVInversed.y * Input.UV.y);
    return Output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_63 [[buffer(0)]])
{
    main0_out out = {};
    VS_Input Input;
    Input.Pos = in.Input_Pos;
    Input.Color = in.Input_Color;
    Input.UV = in.Input_UV;
    Input.Binormal = in.Input_Binormal;
    Input.Tangent = in.Input_Tangent;
    VS_Output flattenTemp = _main(Input, v_63);
    out.gl_Position = flattenTemp.Position;
    out._entryPointOutput_Color = flattenTemp.Color;
    out._entryPointOutput_UV = flattenTemp.UV;
    out._entryPointOutput_Pos = flattenTemp.Pos;
    out._entryPointOutput_PosU = flattenTemp.PosU;
    out._entryPointOutput_PosR = flattenTemp.PosR;
    return out;
}

)";

