static const char metal_sprite_unlit_vs[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_Input
{
    float3 Pos;
    float4 Color;
    float2 UV;
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
    float4x4 mCamera;
    float4x4 mCameraProj;
    float4 mUVInversed;
    float4 mflipbookParameter;
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
    float4 Input_Color [[attribute(1)]];
    float2 Input_UV [[attribute(2)]];
};

static inline __attribute__((always_inline))
VS_Output _main(VS_Input Input, constant VS_ConstantBuffer& v_39)
{
    VS_Output Output = VS_Output{ float4(0.0), float4(0.0), float2(0.0), float4(0.0) };
    float4 worldPos = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    Output.PosVS = v_39.mCameraProj * worldPos;
    Output.Color = Input.Color;
    float2 uv1 = Input.UV;
    uv1.y = v_39.mUVInversed.x + (v_39.mUVInversed.y * uv1.y);
    Output.UV = uv1;
    Output.PosP = Output.PosVS;
    return Output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_39 [[buffer(0)]])
{
    main0_out out = {};
    VS_Input Input;
    Input.Pos = in.Input_Pos;
    Input.Color = in.Input_Color;
    Input.UV = in.Input_UV;
    VS_Output flattenTemp = _main(Input, v_39);
    out.gl_Position = flattenTemp.PosVS;
    out._entryPointOutput_Color = flattenTemp.Color;
    out._entryPointOutput_UV = flattenTemp.UV;
    out._entryPointOutput_PosP = flattenTemp.PosP;
    return out;
}

)";

