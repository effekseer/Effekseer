struct PS_Input
{
    float4 PosVS;
    float4 VColor;
    float2 UV1;
    float2 UV2;
    float3 WorldP;
    float3 WorldN;
    float3 WorldT;
    float3 WorldB;
    float2 ScreenUV;
    float4 PosP;
};

struct FalloffParameter
{
    float4 Param;
    float4 BeginColor;
    float4 EndColor;
};

cbuffer PS_ConstanBuffer : register(b1)
{
    float4 _130_fLightDirection : packoffset(c0);
    float4 _130_fLightColor : packoffset(c1);
    float4 _130_fLightAmbient : packoffset(c2);
    float4 _130_fFlipbookParameter : packoffset(c3);
    float4 _130_fUVDistortionParameter : packoffset(c4);
    float4 _130_fBlendTextureParameter : packoffset(c5);
    float4 _130_fCameraFrontDirection : packoffset(c6);
    FalloffParameter _130_fFalloffParam : packoffset(c7);
    float4 _130_fEmissiveScaling : packoffset(c10);
    float4 _130_fEdgeColor : packoffset(c11);
    float4 _130_fEdgeParameter : packoffset(c12);
    float4 _130_softParticleAndReconstructionParam1 : packoffset(c13);
    float4 _130_reconstructionParam2 : packoffset(c14);
};

Texture2D<float4> _normalTex : register(t1);
SamplerState sampler_normalTex : register(s1);
Texture2D<float4> _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);
Texture2D<float4> _depthTex : register(t2);
SamplerState sampler_depthTex : register(s2);

static float4 gl_FragCoord;
static float4 Input_VColor;
static float2 Input_UV1;
static float2 Input_UV2;
static float3 Input_WorldP;
static float3 Input_WorldN;
static float3 Input_WorldT;
static float3 Input_WorldB;
static float2 Input_ScreenUV;
static float4 Input_PosP;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_VColor : TEXCOORD0;
    centroid float2 Input_UV1 : TEXCOORD1;
    centroid float2 Input_UV2 : TEXCOORD2;
    float3 Input_WorldP : TEXCOORD3;
    float3 Input_WorldN : TEXCOORD4;
    float3 Input_WorldT : TEXCOORD5;
    float3 Input_WorldB : TEXCOORD6;
    float2 Input_ScreenUV : TEXCOORD7;
    float4 Input_PosP : TEXCOORD8;
    float4 gl_FragCoord : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : SV_Target0;
};

float SoftParticle(float backgroundZ, float meshZ, float softparticleParam, float2 reconstruct1, float4 reconstruct2)
{
    float _distance = softparticleParam;
    float2 rescale = reconstruct1;
    float4 params = reconstruct2;
    float2 zs = float2((backgroundZ * rescale.x) + rescale.y, meshZ);
    float2 depth = ((zs * params.w) - params.y.xx) / (params.x.xx - (zs * params.z));
    return min(max((depth.y - depth.x) / _distance, 0.0f), 1.0f);
}

float4 _main(PS_Input Input)
{
    float3 loN = _normalTex.Sample(sampler_normalTex, Input.UV1).xyz;
    float3 texNormal = (loN - 0.5f.xxx) * 2.0f;
    float3 localNormal = normalize(mul(texNormal, float3x3(float3(Input.WorldT), float3(Input.WorldB), float3(Input.WorldN))));
    float diffuse = max(dot(_130_fLightDirection.xyz, localNormal), 0.0f);
    float4 Output = _colorTex.Sample(sampler_colorTex, Input.UV1) * Input.VColor;
    float3 _164 = Output.xyz * ((_130_fLightColor.xyz * diffuse) + float3(_130_fLightAmbient.xyz));
    Output = float4(_164.x, _164.y, _164.z, Output.w);
    float4 screenPos = Input.PosP / Input.PosP.w.xxxx;
    float2 screenUV = (screenPos.xy + 1.0f.xx) / 2.0f.xx;
    screenUV.y = 1.0f - screenUV.y;
    float backgroundZ = _depthTex.Sample(sampler_depthTex, screenUV).x;
    if (_130_softParticleAndReconstructionParam1.x != 0.0f)
    {
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float param_2 = _130_softParticleAndReconstructionParam1.x;
        float2 param_3 = _130_softParticleAndReconstructionParam1.yz;
        float4 param_4 = _130_reconstructionParam2;
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
    Input.VColor = Input_VColor;
    Input.UV1 = Input_UV1;
    Input.UV2 = Input_UV2;
    Input.WorldP = Input_WorldP;
    Input.WorldN = Input_WorldN;
    Input.WorldT = Input_WorldT;
    Input.WorldB = Input_WorldB;
    Input.ScreenUV = Input_ScreenUV;
    Input.PosP = Input_PosP;
    float4 _272 = _main(Input);
    _entryPointOutput = _272;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_VColor = stage_input.Input_VColor;
    Input_UV1 = stage_input.Input_UV1;
    Input_UV2 = stage_input.Input_UV2;
    Input_WorldP = stage_input.Input_WorldP;
    Input_WorldN = stage_input.Input_WorldN;
    Input_WorldT = stage_input.Input_WorldT;
    Input_WorldB = stage_input.Input_WorldB;
    Input_ScreenUV = stage_input.Input_ScreenUV;
    Input_PosP = stage_input.Input_PosP;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
