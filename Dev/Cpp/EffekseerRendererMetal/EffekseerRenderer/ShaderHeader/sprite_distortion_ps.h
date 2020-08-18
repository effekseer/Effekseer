static const char metal_sprite_distortion_ps[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
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
    float4 g_scale;
    float4 mUVInversedBack;
    float4 flipbookParameter;
    float4 uvDistortionParameter;
    float4 blendTextureParameter;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float4 Input_Color [[user(locn0)]];
    float2 Input_UV [[user(locn1)]];
    float4 Input_Pos [[user(locn2)]];
    float4 Input_PosU [[user(locn3)]];
    float4 Input_PosR [[user(locn4)]];
};

static inline __attribute__((always_inline))
float4 _main(PS_Input Input, thread texture2d<float> g_texture, thread sampler g_sampler, constant VS_ConstantBuffer& v_73, thread texture2d<float> g_backTexture, thread sampler g_backSampler)
{
    float4 Output = g_texture.sample(g_sampler, Input.UV);
    Output.w *= Input.Color.w;
    float2 pos = Input.Pos.xy / float2(Input.Pos.w);
    float2 posU = Input.PosU.xy / float2(Input.PosU.w);
    float2 posR = Input.PosR.xy / float2(Input.PosR.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * v_73.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * v_73.g_scale.x;
    float2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = v_73.mUVInversedBack.x + (v_73.mUVInversedBack.y * uv.y);
    float3 color = float3(g_backTexture.sample(g_backSampler, uv).xyz);
    Output = float4(color.x, color.y, color.z, Output.w);
    if (Output.w == 0.0)
    {
        discard_fragment();
    }
    return Output;
}

fragment main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_73 [[buffer(0)]], texture2d<float> g_texture [[texture(0)]], texture2d<float> g_backTexture [[texture(1)]], sampler g_sampler [[sampler(0)]], sampler g_backSampler [[sampler(1)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.Position = gl_FragCoord;
    Input.Color = in.Input_Color;
    Input.UV = in.Input_UV;
    Input.Pos = in.Input_Pos;
    Input.PosU = in.Input_PosU;
    Input.PosR = in.Input_PosR;
    float4 _178 = _main(Input, g_texture, g_sampler, v_73, g_backTexture, g_backSampler);
    out._entryPointOutput = _178;
    return out;
}

)";

