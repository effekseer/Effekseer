struct PS_Input
{
    float4 Position;
    float4 Color;
    float2 UV;
    float4 Pos;
    float4 PosU;
    float4 PosR;
};

cbuffer VS_ConstantBuffer : register(b1)
{
    float4 _73_g_scale : packoffset(c0);
    float4 _73_mUVInversedBack : packoffset(c1);
    float4 _73_flipbookParameter : packoffset(c2);
    float4 _73_uvDistortionParameter : packoffset(c3);
    float4 _73_blendTextureParameter : packoffset(c4);
};

Texture2D<float4> g_texture : register(t0);
SamplerState g_sampler : register(s0);
Texture2D<float4> g_backTexture : register(t1);
SamplerState g_backSampler : register(s1);

static float4 gl_FragCoord;
static float4 Input_Color;
static float2 Input_UV;
static float4 Input_Pos;
static float4 Input_PosU;
static float4 Input_PosR;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float2 Input_UV : TEXCOORD1;
    float4 Input_Pos : TEXCOORD2;
    float4 Input_PosU : TEXCOORD3;
    float4 Input_PosR : TEXCOORD4;
    float4 gl_FragCoord : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : SV_Target0;
};

float4 _main(PS_Input Input)
{
    float4 Output = g_texture.Sample(g_sampler, Input.UV);
    Output.w *= Input.Color.w;
    float2 pos = Input.Pos.xy / Input.Pos.w.xx;
    float2 posU = Input.PosU.xy / Input.PosU.w.xx;
    float2 posR = Input.PosR.xy / Input.PosR.w.xx;
    float xscale = (((Output.x * 2.0f) - 1.0f) * Input.Color.x) * _73_g_scale.x;
    float yscale = (((Output.y * 2.0f) - 1.0f) * Input.Color.y) * _73_g_scale.x;
    float2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0f) * 0.5f;
    uv.y = 1.0f - ((uv.y + 1.0f) * 0.5f);
    uv.y = _73_mUVInversedBack.x + (_73_mUVInversedBack.y * uv.y);
    float3 color = float3(g_backTexture.Sample(g_backSampler, uv).xyz);
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
    Input.Position = gl_FragCoord;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.Pos = Input_Pos;
    Input.PosU = Input_PosU;
    Input.PosR = Input_PosR;
    float4 _178 = _main(Input);
    _entryPointOutput = _178;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    Input_Pos = stage_input.Input_Pos;
    Input_PosU = stage_input.Input_PosU;
    Input_PosR = stage_input.Input_PosR;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
