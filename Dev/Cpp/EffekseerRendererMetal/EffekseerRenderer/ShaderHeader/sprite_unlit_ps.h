static const char metal_sprite_unlit_ps[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float2 UV;
    float4 PosP;
};

struct PS_ConstanBuffer
{
    float4 flipbookParameter;
    float4 uvDistortionParameter;
    float4 blendTextureParameter;
    float4 emissiveScaling;
    float4 edgeColor;
    float4 edgeParameter;
    float4 softParticleAndReconstructionParam1;
    float4 reconstructionParam2;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float4 Input_Color [[user(locn0), centroid_perspective]];
    float2 Input_UV [[user(locn1), centroid_perspective]];
    float4 Input_PosP [[user(locn2)]];
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
float4 _main(PS_Input Input, thread texture2d<float> _colorTex, thread sampler sampler_colorTex, thread texture2d<float> _depthTex, thread sampler sampler_depthTex, constant PS_ConstanBuffer& v_124)
{
    float4 Output = Input.Color * _colorTex.sample(sampler_colorTex, Input.UV);
    float4 screenPos = Input.PosP / float4(Input.PosP.w);
    float2 screenUV = (screenPos.xy + float2(1.0)) / float2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    float backgroundZ = _depthTex.sample(sampler_depthTex, screenUV).x;
    if ((isunordered(v_124.softParticleAndReconstructionParam1.x, 0.0) || v_124.softParticleAndReconstructionParam1.x != 0.0))
    {
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float param_2 = v_124.softParticleAndReconstructionParam1.x;
        float2 param_3 = v_124.softParticleAndReconstructionParam1.yz;
        float4 param_4 = v_124.reconstructionParam2;
        Output.w *= SoftParticle(param, param_1, param_2, param_3, param_4);
    }
    if (Output.w == 0.0)
    {
        discard_fragment();
    }
    return Output;
}

fragment main0_out main0(main0_in in [[stage_in]], constant PS_ConstanBuffer& v_124 [[buffer(0)]], texture2d<float> _colorTex [[texture(0)]], texture2d<float> _depthTex [[texture(1)]], sampler sampler_colorTex [[sampler(0)]], sampler sampler_depthTex [[sampler(1)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = in.Input_Color;
    Input.UV = in.Input_UV;
    Input.PosP = in.Input_PosP;
    float4 _184 = _main(Input, _colorTex, sampler_colorTex, _depthTex, sampler_depthTex, v_124);
    out._entryPointOutput = _184;
    return out;
}

)";

