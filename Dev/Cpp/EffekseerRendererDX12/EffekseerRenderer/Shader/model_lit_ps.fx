struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float2 UV;
    float3 WorldN;
    float3 WorldB;
    float3 WorldT;
    float4 PosP;
};

cbuffer PS_ConstanBuffer : register(b1)
{
    float4 _195_fLightDirection : packoffset(c0);
    float4 _195_fLightColor : packoffset(c1);
    float4 _195_fLightAmbient : packoffset(c2);
    float4 _195_fFlipbookParameter : packoffset(c3);
    float4 _195_fUVDistortionParameter : packoffset(c4);
    float4 _195_fBlendTextureParameter : packoffset(c5);
    float4 _195_fCameraFrontDirection : packoffset(c6);
    float4 _195_fFalloffParameter : packoffset(c7);
    float4 _195_fFalloffBeginColor : packoffset(c8);
    float4 _195_fFalloffEndColor : packoffset(c9);
    float4 _195_fEmissiveScaling : packoffset(c10);
    float4 _195_fEdgeColor : packoffset(c11);
    float4 _195_fEdgeParameter : packoffset(c12);
    float4 _195_softParticleParam : packoffset(c13);
    float4 _195_reconstructionParam1 : packoffset(c14);
    float4 _195_reconstructionParam2 : packoffset(c15);
    float4 _195_mUVInversedBack : packoffset(c16);
    float4 _195_miscFlags : packoffset(c17);
};

Texture2D<float4> _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);
Texture2D<float4> _normalTex : register(t1);
SamplerState sampler_normalTex : register(s1);
Texture2D<float4> _depthTex : register(t2);
SamplerState sampler_depthTex : register(s2);

static float4 gl_FragCoord;
static float4 Input_Color;
static float2 Input_UV;
static float3 Input_WorldN;
static float3 Input_WorldB;
static float3 Input_WorldT;
static float4 Input_PosP;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float2 Input_UV : TEXCOORD1;
    float3 Input_WorldN : TEXCOORD2;
    float3 Input_WorldB : TEXCOORD3;
    float3 Input_WorldT : TEXCOORD4;
    float4 Input_PosP : TEXCOORD5;
    float4 gl_FragCoord : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : SV_Target0;
};

float3 PositivePow(float3 base, float3 power)
{
    return pow(max(abs(base), 1.1920928955078125e-07f.xxx), power);
}

float3 LinearToSRGB(float3 c)
{
    float3 param = c;
    float3 param_1 = 0.4166666567325592041015625f.xxx;
    return max((PositivePow(param, param_1) * 1.05499994754791259765625f) - 0.054999999701976776123046875f.xxx, 0.0f.xxx);
}

float4 LinearToSRGB(float4 c)
{
    float3 param = c.xyz;
    return float4(LinearToSRGB(param), c.w);
}

float4 ConvertFromSRGBTexture(float4 c)
{
    if (_195_miscFlags.x == 0.0f)
    {
        return c;
    }
    float4 param = c;
    return LinearToSRGB(param);
}

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

float3 SRGBToLinear(float3 c)
{
    return c * ((c * ((c * 0.305306017398834228515625f) + 0.6821711063385009765625f.xxx)) + 0.01252287812530994415283203125f.xxx);
}

float4 SRGBToLinear(float4 c)
{
    float3 param = c.xyz;
    return float4(SRGBToLinear(param), c.w);
}

float4 ConvertToScreen(float4 c)
{
    if (_195_miscFlags.x == 0.0f)
    {
        return c;
    }
    float4 param = c;
    return SRGBToLinear(param);
}

float4 _main(PS_Input Input)
{
    float4 param = _colorTex.Sample(sampler_colorTex, Input.UV);
    float4 Output = ConvertFromSRGBTexture(param) * Input.Color;
    float3 texNormal = (_normalTex.Sample(sampler_normalTex, Input.UV).xyz - 0.5f.xxx) * 2.0f;
    float3 localNormal = normalize(mul(texNormal, float3x3(float3(Input.WorldT), float3(Input.WorldB), float3(Input.WorldN))));
    float diffuse = max(dot(_195_fLightDirection.xyz, localNormal), 0.0f);
    float3 _301 = Output.xyz * ((_195_fLightColor.xyz * diffuse) + _195_fLightAmbient.xyz);
    Output = float4(_301.x, _301.y, _301.z, Output.w);
    float3 _309 = Output.xyz * _195_fEmissiveScaling.x;
    Output = float4(_309.x, _309.y, _309.z, Output.w);
    float4 screenPos = Input.PosP / Input.PosP.w.xxxx;
    float2 screenUV = (screenPos.xy + 1.0f.xx) / 2.0f.xx;
    screenUV.y = 1.0f - screenUV.y;
    screenUV.y = _195_mUVInversedBack.x + (_195_mUVInversedBack.y * screenUV.y);
    if (_195_softParticleParam.w != 0.0f)
    {
        float backgroundZ = _depthTex.Sample(sampler_depthTex, screenUV).x;
        float param_1 = backgroundZ;
        float param_2 = screenPos.z;
        float4 param_3 = _195_softParticleParam;
        float4 param_4 = _195_reconstructionParam1;
        float4 param_5 = _195_reconstructionParam2;
        Output.w *= SoftParticle(param_1, param_2, param_3, param_4, param_5);
    }
    if (Output.w == 0.0f)
    {
        discard;
    }
    float4 param_6 = Output;
    return ConvertToScreen(param_6);
}

void frag_main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.WorldN = Input_WorldN;
    Input.WorldB = Input_WorldB;
    Input.WorldT = Input_WorldT;
    Input.PosP = Input_PosP;
    float4 _415 = _main(Input);
    _entryPointOutput = _415;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    Input_WorldN = stage_input.Input_WorldN;
    Input_WorldB = stage_input.Input_WorldB;
    Input_WorldT = stage_input.Input_WorldT;
    Input_PosP = stage_input.Input_PosP;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
