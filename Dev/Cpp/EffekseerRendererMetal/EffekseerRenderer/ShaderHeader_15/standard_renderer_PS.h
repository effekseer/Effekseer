static const char metal_standard_renderer_PS[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
{
    float4 Pos;
    float4 Color;
    float2 UV;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float4 Input_Color [[user(locn0)]];
    float2 Input_UV [[user(locn1)]];
};

static inline __attribute__((always_inline))
float4 _main(PS_Input Input, thread texture2d<float> g_texture, thread sampler g_sampler)
{
    float4 Output = Input.Color * g_texture.sample(g_sampler, Input.UV);
    if (Output.w == 0.0)
    {
        discard_fragment();
    }
    return Output;
}

fragment main0_out main0(main0_in in [[stage_in]], texture2d<float> g_texture [[texture(0)]], sampler g_sampler [[sampler(0)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.Color = in.Input_Color;
    Input.UV = in.Input_UV;
    float4 _65 = _main(Input, g_texture, g_sampler);
    out._entryPointOutput = _65;
    return out;
}

)";

