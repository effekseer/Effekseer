struct PS_Input
{
    float4 Pos;
    float4 Color;
    float2 UV;
};

cbuffer PS_ConstanBuffer : register(b1)
{
    float4 _68_flipbookParameter : packoffset(c0);
    float4 _68_uvDistortionParameter : packoffset(c1);
    float4 _68_blendTextureParameter : packoffset(c2);
    float4 _68_emissiveScaling : packoffset(c3);
    float4 _68_edgeColor : packoffset(c4);
    float4 _68_edgeParameter : packoffset(c5);
};

Texture2D<float4> g_texture : register(t0);
SamplerState g_sampler : register(s0);

static float4 gl_FragCoord;
static float4 Input_Color;
static float2 Input_UV;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    float4 Input_Color : TEXCOORD0;
    float2 Input_UV : TEXCOORD1;
    float4 gl_FragCoord : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : SV_Target0;
};

float4 _main(PS_Input Input)
{
    float4 Output = Input.Color * g_texture.Sample(g_sampler, Input.UV);
    if (Output.w == 0.0f)
    {
        discard;
    }
    return Output;
}

void frag_main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    float4 _65 = _main(Input);
    _entryPointOutput = _65;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
