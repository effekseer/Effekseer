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

cbuffer PS_ConstanBuffer : register(b1)
{
    float4 _129_g_scale : packoffset(c0);
    float4 _129_mUVInversedBack : packoffset(c1);
    float4 _129_fFlipbookParameter : packoffset(c2);
    float4 _129_fUVDistortionParameter : packoffset(c3);
    float4 _129_fBlendTextureParameter : packoffset(c4);
    float4 _129_softParticleAndReconstructionParam1 : packoffset(c5);
    float4 _129_reconstructionParam2 : packoffset(c6);
};

Texture2D<float4> g_texture : register(t0);
SamplerState g_sampler : register(s0);
Texture2D<float4> g_backTexture : register(t1);
SamplerState g_backSampler : register(s1);
Texture2D<float4> g_depthTexture : register(t2);
SamplerState g_depthSampler : register(s2);

static float4 gl_FragCoord;
static float2 Input_UV;
static float4 Input_Normal;
static float4 Input_Binormal;
static float4 Input_Tangent;
static float4 Input_PosP;
static float4 Input_Color;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float2 Input_UV : TEXCOORD0;
    float4 Input_Normal : TEXCOORD1;
    float4 Input_Binormal : TEXCOORD2;
    float4 Input_Tangent : TEXCOORD3;
    float4 Input_PosP : TEXCOORD4;
    centroid float4 Input_Color : TEXCOORD5;
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
    float4 Output = g_texture.Sample(g_sampler, Input.UV);
    Output.w *= Input.Color.w;
    float2 pos = Input.PosP.xy / Input.PosP.w.xx;
    float2 posU = Input.Tangent.xy / Input.Tangent.w.xx;
    float2 posR = Input.Binormal.xy / Input.Binormal.w.xx;
    float xscale = (((Output.x * 2.0f) - 1.0f) * Input.Color.x) * _129_g_scale.x;
    float yscale = (((Output.y * 2.0f) - 1.0f) * Input.Color.y) * _129_g_scale.x;
    float2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0f) * 0.5f;
    uv.y = 1.0f - ((uv.y + 1.0f) * 0.5f);
    uv.y = _129_mUVInversedBack.x + (_129_mUVInversedBack.y * uv.y);
    float3 color = float3(g_backTexture.Sample(g_backSampler, uv).xyz);
    Output = float4(color.x, color.y, color.z, Output.w);
    float4 screenPos = Input.PosP / Input.PosP.w.xxxx;
    float2 screenUV = (screenPos.xy + 1.0f.xx) / 2.0f.xx;
    screenUV.y = 1.0f - screenUV.y;
    float backgroundZ = g_depthTexture.Sample(g_depthSampler, screenUV).x;
    if (_129_softParticleAndReconstructionParam1.x != 0.0f)
    {
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float param_2 = _129_softParticleAndReconstructionParam1.x;
        float2 param_3 = _129_softParticleAndReconstructionParam1.yz;
        float4 param_4 = _129_reconstructionParam2;
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
    Input.PosP = Input_PosP;
    Input.Color = Input_Color;
    float4 _287 = _main(Input);
    _entryPointOutput = _287;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_UV = stage_input.Input_UV;
    Input_Normal = stage_input.Input_Normal;
    Input_Binormal = stage_input.Input_Binormal;
    Input_Tangent = stage_input.Input_Tangent;
    Input_PosP = stage_input.Input_PosP;
    Input_Color = stage_input.Input_Color;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
