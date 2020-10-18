#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
{
    float4 PosVS;
    float2 UV;
    float4 Normal;
    float4 Binormal;
    float4 Tangent;
    float4 PosP;
    float4 Color;
};

struct PS_ConstanBuffer
{
    float4 g_scale;
    float4 mUVInversedBack;
    float4 fFlipbookParameter;
    float4 fUVDistortionParameter;
    float4 fBlendTextureParameter;
    float4 softParticleAndReconstructionParam1;
    float4 reconstructionParam2;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float2 Input_UV [[user(locn0), centroid_perspective]];
    float4 Input_Normal [[user(locn1)]];
    float4 Input_Binormal [[user(locn2)]];
    float4 Input_Tangent [[user(locn3)]];
    float4 Input_PosP [[user(locn4)]];
    float4 Input_Color [[user(locn5), centroid_perspective]];
};

static inline __attribute__((always_inline))
float SoftParticle(thread const float& backgroundZ, thread const float& meshZ, thread const float& softparticleParam, thread const float2& reconstruct1, thread const float4& reconstruct2)
{
    float _distance = softparticleParam;
    float2 rescale = reconstruct1;
    float4 params = reconstruct2;
    float2 zs = float2((backgroundZ * rescale.x) + rescale.y, meshZ);
    float2 depth = ((zs * params.w) - float2(params.y)) / (float2(params.x) - (zs * params.z));
    return fast::min(fast::max((depth.y - depth.x) / _distance, 0.0), 1.0);
}

static inline __attribute__((always_inline))
float4 _main(PS_Input Input, thread texture2d<float> g_texture, thread sampler g_sampler, constant PS_ConstanBuffer& v_129, thread texture2d<float> g_backTexture, thread sampler g_backSampler, thread texture2d<float> g_depthTexture, thread sampler g_depthSampler)
{
    float4 Output = g_texture.sample(g_sampler, Input.UV);
    Output.w *= Input.Color.w;
    float2 pos = Input.PosP.xy / float2(Input.PosP.w);
    float2 posU = Input.Tangent.xy / float2(Input.Tangent.w);
    float2 posR = Input.Binormal.xy / float2(Input.Binormal.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * v_129.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * v_129.g_scale.x;
    float2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = v_129.mUVInversedBack.x + (v_129.mUVInversedBack.y * uv.y);
    float3 color = float3(g_backTexture.sample(g_backSampler, uv).xyz);
    Output = float4(color.x, color.y, color.z, Output.w);
    float4 screenPos = Input.PosP / float4(Input.PosP.w);
    float2 screenUV = (screenPos.xy + float2(1.0)) / float2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    float backgroundZ = g_depthTexture.sample(g_depthSampler, screenUV).x;
    if ((isunordered(v_129.softParticleAndReconstructionParam1.x, 0.0) || v_129.softParticleAndReconstructionParam1.x != 0.0))
    {
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float param_2 = v_129.softParticleAndReconstructionParam1.x;
        float2 param_3 = v_129.softParticleAndReconstructionParam1.yz;
        float4 param_4 = v_129.reconstructionParam2;
        Output.w *= SoftParticle(param, param_1, param_2, param_3, param_4);
    }
    if (Output.w == 0.0)
    {
        discard_fragment();
    }
    return Output;
}

fragment main0_out main0(main0_in in [[stage_in]], constant PS_ConstanBuffer& v_129 [[buffer(0)]], texture2d<float> g_texture [[texture(0)]], texture2d<float> g_backTexture [[texture(1)]], texture2d<float> g_depthTexture [[texture(2)]], sampler g_sampler [[sampler(0)]], sampler g_backSampler [[sampler(1)]], sampler g_depthSampler [[sampler(2)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.UV = in.Input_UV;
    Input.Normal = in.Input_Normal;
    Input.Binormal = in.Input_Binormal;
    Input.Tangent = in.Input_Tangent;
    Input.PosP = in.Input_PosP;
    Input.Color = in.Input_Color;
    float4 _287 = _main(Input, g_texture, g_sampler, v_129, g_backTexture, g_backSampler, g_depthTexture, g_depthSampler);
    out._entryPointOutput = _287;
    return out;
}

