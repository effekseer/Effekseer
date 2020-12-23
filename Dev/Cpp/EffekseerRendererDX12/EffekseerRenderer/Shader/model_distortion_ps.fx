struct PS_Input
{
    float4 PosVS;
    float2 UV;
    float4 ProjBinormal;
    float4 ProjTangent;
    float4 PosP;
    float4 Color;
};

cbuffer PS_ConstanBuffer : register(b1)
{
    float4 _149_g_scale : packoffset(c0);
    float4 _149_mUVInversedBack : packoffset(c1);
    float4 _149_fFlipbookParameter : packoffset(c2);
    float4 _149_fUVDistortionParameter : packoffset(c3);
    float4 _149_fBlendTextureParameter : packoffset(c4);
    float4 _149_softParticleParam : packoffset(c5);
    float4 _149_reconstructionParam1 : packoffset(c6);
    float4 _149_reconstructionParam2 : packoffset(c7);
};

Texture2D<float4> _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);
Texture2D<float4> _backTex : register(t1);
SamplerState sampler_backTex : register(s1);
Texture2D<float4> _depthTex : register(t2);
SamplerState sampler_depthTex : register(s2);

static float4 gl_FragCoord;
static float2 Input_UV;
static float4 Input_ProjBinormal;
static float4 Input_ProjTangent;
static float4 Input_PosP;
static float4 Input_Color;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float2 Input_UV : TEXCOORD0;
    float4 Input_ProjBinormal : TEXCOORD1;
    float4 Input_ProjTangent : TEXCOORD2;
    float4 Input_PosP : TEXCOORD3;
    centroid float4 Input_Color : TEXCOORD4;
    float4 gl_FragCoord : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : SV_Target0;
};

float SoftParticle(float backgroundZ, float meshZ, float4 softparticleParam, float4 reconstruct1, float4 reconstruct2)
{
    float distanceFar = softparticleParam.x;
    float distanceNear = softparticleParam.y;
    float distanceNearOffset = softparticleParam.z;
    float2 rescale = reconstruct1.xy;
    float4 params = reconstruct2;
    float2 zs = float2((backgroundZ * rescale.x) + rescale.y, meshZ);
    float2 depth = ((zs * params.w) - params.y.xx) / (params.x.xx - (zs * params.z));
    float alphaFar = (depth.y - depth.x) / distanceFar;
    float alphaNear = ((-distanceNearOffset) - depth.y) / distanceNear;
    return min(max(min(alphaFar, alphaNear), 0.0f), 1.0f);
}

float4 _main(PS_Input Input)
{
    float4 Output = _colorTex.Sample(sampler_colorTex, Input.UV);
    Output.w *= Input.Color.w;
    float2 pos = Input.PosP.xy / Input.PosP.w.xx;
    float2 posR = Input.ProjTangent.xy / Input.ProjTangent.w.xx;
    float2 posU = Input.ProjBinormal.xy / Input.ProjBinormal.w.xx;
    float xscale = (((Output.x * 2.0f) - 1.0f) * Input.Color.x) * _149_g_scale.x;
    float yscale = (((Output.y * 2.0f) - 1.0f) * Input.Color.y) * _149_g_scale.x;
    float2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0f) * 0.5f;
    uv.y = 1.0f - ((uv.y + 1.0f) * 0.5f);
    uv.y = _149_mUVInversedBack.x + (_149_mUVInversedBack.y * uv.y);
    float3 color = float3(_backTex.Sample(sampler_backTex, uv).xyz);
    Output = float4(color.x, color.y, color.z, Output.w);
    float4 screenPos = Input.PosP / Input.PosP.w.xxxx;
    float2 screenUV = (screenPos.xy + 1.0f.xx) / 2.0f.xx;
    screenUV.y = 1.0f - screenUV.y;
    if (_149_softParticleParam.w != 0.0f)
    {
        float backgroundZ = _depthTex.Sample(sampler_depthTex, screenUV).x;
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float4 param_2 = _149_softParticleParam;
        float4 param_3 = _149_reconstructionParam1;
        float4 param_4 = _149_reconstructionParam2;
        Output.w *= SoftParticle(param, param_1, param_2, param_3, param_4);
    }
    if (Output.w == 0.0f)
    {
        discard;
    }
    return Output;
}

void frag_main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.UV = Input_UV;
    Input.ProjBinormal = Input_ProjBinormal;
    Input.ProjTangent = Input_ProjTangent;
    Input.PosP = Input_PosP;
    Input.Color = Input_Color;
    float4 _304 = _main(Input);
    _entryPointOutput = _304;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_UV = stage_input.Input_UV;
    Input_ProjBinormal = stage_input.Input_ProjBinormal;
    Input_ProjTangent = stage_input.Input_ProjTangent;
    Input_PosP = stage_input.Input_PosP;
    Input_Color = stage_input.Input_Color;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
