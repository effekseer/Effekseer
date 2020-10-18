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

cbuffer VS_ConstantBuffer : register(b1)
{
    float4 _138_fLightDirection : packoffset(c0);
    float4 _138_fLightColor : packoffset(c1);
    float4 _138_fLightAmbient : packoffset(c2);
    float4 _138_softParticleAndReconstructionParam1 : packoffset(c3);
    float4 _138_reconstructionParam2 : packoffset(c4);
};

Texture2D<float4> g_colorTexture : register(t0);
SamplerState g_colorSampler : register(s0);
Texture2D<float4> g_normalTexture : register(t1);
SamplerState g_normalSampler : register(s1);
Texture2D<float4> g_depthTexture : register(t2);
SamplerState g_depthSampler : register(s2);

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
    float4 Output = g_colorTexture.Sample(g_colorSampler, Input.UV) * Input.Color;
    float3 texNormal = (g_normalTexture.Sample(g_normalSampler, Input.UV).xyz - 0.5f.xxx) * 2.0f;
    float3 localNormal = normalize(mul(texNormal, float3x3(float3(Input.Tangent), float3(Input.Binormal), float3(Input.Normal))));
    float diffuse = max(dot(_138_fLightDirection.xyz, localNormal), 0.0f);
    float3 _158 = Output.xyz * ((_138_fLightColor.xyz * diffuse) + _138_fLightAmbient.xyz);
    Output = float4(_158.x, _158.y, _158.z, Output.w);
    float4 screenPos = Input.PosP / Input.PosP.w.xxxx;
    float2 screenUV = (screenPos.xy + 1.0f.xx) / 2.0f.xx;
    screenUV.y = 1.0f - screenUV.y;
    float backgroundZ = g_depthTexture.Sample(g_depthSampler, screenUV).x;
    if (_138_softParticleAndReconstructionParam1.x != 0.0f)
    {
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float param_2 = _138_softParticleAndReconstructionParam1.x;
        float2 param_3 = _138_softParticleAndReconstructionParam1.yz;
        float4 param_4 = _138_reconstructionParam2;
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
    float4 _252 = _main(Input);
    _entryPointOutput = _252;
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
