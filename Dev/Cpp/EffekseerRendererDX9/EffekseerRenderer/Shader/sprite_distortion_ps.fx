struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float2 UV;
    float4 PosP;
    float4 PosU;
    float4 PosR;
};

cbuffer VS_ConstantBuffer : register(b0)
{
    float4 _73_g_scale : register(c0);
    float4 _73_mUVInversedBack : register(c1);
    float4 _73_flipbookParameter : register(c2);
    float4 _73_uvDistortionParameter : register(c3);
    float4 _73_blendTextureParameter : register(c4);
    float4 _73_softParticleAndReconstructionParam1 : register(c5);
    float4 _73_reconstructionParam2 : register(c6);
};

uniform sampler2D Sampler_g_sampler : register(s0);
uniform sampler2D Sampler_g_backSampler : register(s1);

static float4 gl_FragCoord;
static float4 Input_Color;
static float2 Input_UV;
static float4 Input_PosP;
static float4 Input_PosU;
static float4 Input_PosR;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float2 Input_UV : TEXCOORD1;
    float4 Input_PosP : TEXCOORD2;
    float4 Input_PosU : TEXCOORD3;
    float4 Input_PosR : TEXCOORD4;
    float4 gl_FragCoord : VPOS;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : COLOR0;
};

float4 _main(PS_Input Input)
{
    float4 Output = tex2D(Sampler_g_sampler, Input.UV);
    Output.w *= Input.Color.w;
    float2 pos = Input.PosP.xy / Input.PosP.w.xx;
    float2 posU = Input.PosU.xy / Input.PosU.w.xx;
    float2 posR = Input.PosR.xy / Input.PosR.w.xx;
    float xscale = (((Output.x * 2.0f) - 1.0f) * Input.Color.x) * _73_g_scale.x;
    float yscale = (((Output.y * 2.0f) - 1.0f) * Input.Color.y) * _73_g_scale.x;
    float2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0f) * 0.5f;
    uv.y = 1.0f - ((uv.y + 1.0f) * 0.5f);
    uv.y = _73_mUVInversedBack.x + (_73_mUVInversedBack.y * uv.y);
    float3 color = float3(tex2D(Sampler_g_backSampler, uv).xyz);
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
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.PosP = Input_PosP;
    Input.PosU = Input_PosU;
    Input.PosR = Input_PosR;
    float4 _178 = _main(Input);
    _entryPointOutput = _178;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord + float4(0.5f, 0.5f, 0.0f, 0.0f);
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    Input_PosP = stage_input.Input_PosP;
    Input_PosU = stage_input.Input_PosU;
    Input_PosR = stage_input.Input_PosR;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = float4(_entryPointOutput);
    return stage_output;
}
