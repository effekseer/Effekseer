struct PS_Input
{
    float4 PosVS;
    float2 UV;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float4 Color;
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
    float4 _139_fLightDirection : packoffset(c0);
    float4 _139_fLightColor : packoffset(c1);
    float4 _139_fLightAmbient : packoffset(c2);
    float4 _139_fFlipbookParameter : packoffset(c3);
    float4 _139_fUVDistortionParameter : packoffset(c4);
    float4 _139_fBlendTextureParameter : packoffset(c5);
    float4 _139_fCameraFrontDirection : packoffset(c6);
    FalloffParameter _139_fFalloffParam : packoffset(c7);
    float4 _139_fEmissiveScaling : packoffset(c10);
    float4 _139_fEdgeColor : packoffset(c11);
    float4 _139_fEdgeParameter : packoffset(c12);
    float4 _139_softParticleAndReconstructionParam1 : packoffset(c13);
    float4 _139_reconstructionParam2 : packoffset(c14);
};

Texture2D<float4> _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);
Texture2D<float4> _normalTex : register(t1);
SamplerState sampler_normalTex : register(s1);
Texture2D<float4> _depthTex : register(t2);
SamplerState sampler_depthTex : register(s2);

static float4 gl_FragCoord;
static float2 Input_UV;
static float3 Input_Normal;
static float3 Input_Binormal;
static float3 Input_Tangent;
static float4 Input_Color;
static float4 Input_PosP;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float2 Input_UV : TEXCOORD0;
    float3 Input_Normal : TEXCOORD1;
    float3 Input_Binormal : TEXCOORD2;
    float3 Input_Tangent : TEXCOORD3;
    centroid float4 Input_Color : TEXCOORD4;
    float4 Input_PosP : TEXCOORD5;
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
    float4 Output = _colorTex.Sample(sampler_colorTex, Input.UV) * Input.Color;
    float3 texNormal = (_normalTex.Sample(sampler_normalTex, Input.UV).xyz - 0.5f.xxx) * 2.0f;
    float3 localNormal = normalize(mul(texNormal, float3x3(float3(Input.Tangent), float3(Input.Binormal), float3(Input.Normal))));
    float diffuse = max(dot(_139_fLightDirection.xyz, localNormal), 0.0f);
    float3 _159 = Output.xyz * ((_139_fLightColor.xyz * diffuse) + _139_fLightAmbient.xyz);
    Output = float4(_159.x, _159.y, _159.z, Output.w);
    float4 screenPos = Input.PosP / Input.PosP.w.xxxx;
    float2 screenUV = (screenPos.xy + 1.0f.xx) / 2.0f.xx;
    screenUV.y = 1.0f - screenUV.y;
    float backgroundZ = _depthTex.Sample(sampler_depthTex, screenUV).x;
    if (_139_softParticleAndReconstructionParam1.x != 0.0f)
    {
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float param_2 = _139_softParticleAndReconstructionParam1.x;
        float2 param_3 = _139_softParticleAndReconstructionParam1.yz;
        float4 param_4 = _139_reconstructionParam2;
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
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.Color = Input_Color;
    Input.PosP = Input_PosP;
    float4 _255 = _main(Input);
    _entryPointOutput = _255;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_UV = stage_input.Input_UV;
    Input_Normal = stage_input.Input_Normal;
    Input_Binormal = stage_input.Input_Binormal;
    Input_Tangent = stage_input.Input_Tangent;
    Input_Color = stage_input.Input_Color;
    Input_PosP = stage_input.Input_PosP;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
