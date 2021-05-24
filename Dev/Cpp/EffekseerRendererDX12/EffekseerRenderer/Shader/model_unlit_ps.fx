struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float2 UV;
    float4 PosP;
};

cbuffer PS_ConstanBuffer : register(b1)
{
    float4 _123_fLightDirection : packoffset(c0);
    float4 _123_fLightColor : packoffset(c1);
    float4 _123_fLightAmbient : packoffset(c2);
    float4 _123_fFlipbookParameter : packoffset(c3);
    float4 _123_fUVDistortionParameter : packoffset(c4);
    float4 _123_fBlendTextureParameter : packoffset(c5);
    float4 _123_fCameraFrontDirection : packoffset(c6);
    float4 _123_fFalloffParameter : packoffset(c7);
    float4 _123_fFalloffBeginColor : packoffset(c8);
    float4 _123_fFalloffEndColor : packoffset(c9);
    float4 _123_fEmissiveScaling : packoffset(c10);
    float4 _123_fEdgeColor : packoffset(c11);
    float4 _123_fEdgeParameter : packoffset(c12);
    float4 _123_softParticleParam : packoffset(c13);
    float4 _123_reconstructionParam1 : packoffset(c14);
    float4 _123_reconstructionParam2 : packoffset(c15);
    float4 _123_mUVInversedBack : packoffset(c16);
};

Texture2D<float4> _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);
Texture2D<float4> _depthTex : register(t1);
SamplerState sampler_depthTex : register(s1);

static float4 gl_FragCoord;
static float4 Input_Color;
static float2 Input_UV;
static float4 Input_PosP;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float2 Input_UV : TEXCOORD1;
    float4 Input_PosP : TEXCOORD2;
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
    float dir = sign(depth.x);
    depth *= dir;
    float alphaFar = (depth.x - depth.y) / distanceFar;
    float alphaNear = (depth.y - distanceNearOffset) / distanceNear;
    return min(max(min(alphaFar, alphaNear), 0.0f), 1.0f);
}

float4 _main(PS_Input Input)
{
    float4 Output = _colorTex.Sample(sampler_colorTex, Input.UV) * Input.Color;
    float3 _131 = Output.xyz * _123_fEmissiveScaling.x;
    Output = float4(_131.x, _131.y, _131.z, Output.w);
    float4 screenPos = Input.PosP / Input.PosP.w.xxxx;
    float2 screenUV = (screenPos.xy + 1.0f.xx) / 2.0f.xx;
    screenUV.y = 1.0f - screenUV.y;
    screenUV.y = _123_mUVInversedBack.x + (_123_mUVInversedBack.y * screenUV.y);
    if (_123_softParticleParam.w != 0.0f)
    {
        float backgroundZ = _depthTex.Sample(sampler_depthTex, screenUV).x;
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float4 param_2 = _123_softParticleParam;
        float4 param_3 = _123_reconstructionParam1;
        float4 param_4 = _123_reconstructionParam2;
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
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.PosP = Input_PosP;
    float4 _229 = _main(Input);
    _entryPointOutput = _229;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    Input_PosP = stage_input.Input_PosP;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
