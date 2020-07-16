static const char metal_standard_renderer_VS[] = R"(mtlcode
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
    float4 Position;
    float4 Color;
    float2 UV;
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
    float4 gl_Position [[position]];
};

struct main0_in
{
    float3 Input_Pos [[attribute(0)]];
    float4 Input_Color [[attribute(1)]];
    float2 Input_UV [[attribute(2)]];
};

static inline __attribute__((always_inline))
VS_Output _main(VS_Input Input, constant VS_ConstantBuffer& v_40)
{
    VS_Output Output = VS_Output{ float4(0.0), float4(0.0), float2(0.0) };
    float4 pos4 = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    float4 cameraPos = v_40.mCamera * pos4;
    cameraPos /= float4(cameraPos.w);
    Output.Position = v_40.mProj * cameraPos;
    float4 cameraPosU = cameraPos + float4(0.0, 1.0, 0.0, 0.0);
    float4 cameraPosR = cameraPos + float4(1.0, 0.0, 0.0, 0.0);
    Output.Color = Input.Color;
    Output.UV = Input.UV;
    Output.UV.y = v_40.mUVInversed.x + (v_40.mUVInversed.y * Input.UV.y);
    return Output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_40 [[buffer(0)]])
{
    main0_out out = {};
    VS_Input Input;
    Input.Pos = in.Input_Pos;
    Input.Color = in.Input_Color;
    Input.UV = in.Input_UV;
    VS_Output flattenTemp = _main(Input, v_40);
    out.gl_Position = flattenTemp.Position;
    out._entryPointOutput_Color = flattenTemp.Color;
    out._entryPointOutput_UV = flattenTemp.UV;
    return out;
}

)";

