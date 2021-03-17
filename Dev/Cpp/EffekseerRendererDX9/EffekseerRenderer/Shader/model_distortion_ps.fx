struct PS_Input
{
    float4 PosVS;
    float2 UV;
    float4 ProjBinormal;
    float4 ProjTangent;
    float4 PosP;
    float4 Color;
};

cbuffer PS_ConstanBuffer : register(b0)
{
    float4 _73_g_scale : register(c0);
    float4 _73_mUVInversedBack : register(c1);
    float4 _73_fFlipbookParameter : register(c2);
    float4 _73_fUVDistortionParameter : register(c3);
    float4 _73_fBlendTextureParameter : register(c4);
    float4 _73_softParticleParam : register(c5);
    float4 _73_reconstructionParam1 : register(c6);
    float4 _73_reconstructionParam2 : register(c7);
};

uniform sampler2D Sampler_sampler_colorTex : register(s0);
uniform sampler2D Sampler_sampler_backTex : register(s1);

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
    float4 gl_FragCoord : VPOS;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : COLOR0;
};

float4 _main(PS_Input Input)
{
    float4 Output = tex2D(Sampler_sampler_colorTex, Input.UV);
    Output.w *= Input.Color.w;
    float2 pos = Input.PosP.xy / Input.PosP.w.xx;
    float2 posR = Input.ProjTangent.xy / Input.ProjTangent.w.xx;
    float2 posU = Input.ProjBinormal.xy / Input.ProjBinormal.w.xx;
    float xscale = (((Output.x * 2.0f) - 1.0f) * Input.Color.x) * _73_g_scale.x;
    float yscale = (((Output.y * 2.0f) - 1.0f) * Input.Color.y) * _73_g_scale.x;
    float2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0f) * 0.5f;
    uv.y = 1.0f - ((uv.y + 1.0f) * 0.5f);
    uv.y = _73_mUVInversedBack.x + (_73_mUVInversedBack.y * uv.y);
    float3 color = float3(tex2D(Sampler_sampler_backTex, uv).xyz);
    Output = float4(color.x, color.y, color.z, Output.w);
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
    float4 _178 = _main(Input);
    _entryPointOutput = _178;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord + float4(0.5f, 0.5f, 0.0f, 0.0f);
    Input_UV = stage_input.Input_UV;
    Input_ProjBinormal = stage_input.Input_ProjBinormal;
    Input_ProjTangent = stage_input.Input_ProjTangent;
    Input_PosP = stage_input.Input_PosP;
    Input_Color = stage_input.Input_Color;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = float4(_entryPointOutput);
    return stage_output;
}
