struct PS_Input
{
    float4 Pos;
    float2 UV;
    float4 Color;
};

cbuffer VS_ConstantBuffer : register(b1)
{
    float4 _68_fLightDirection : packoffset(c0);
    float4 _68_fLightColor : packoffset(c1);
    float4 _68_fLightAmbient : packoffset(c2);
};

Texture2D<float4> g_colorTexture : register(t0);
SamplerState g_colorSampler : register(s0);

static float4 gl_FragCoord;
static float2 Input_UV;
static float4 Input_Color;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float2 Input_UV : TEXCOORD0;
    centroid float4 Input_Color : TEXCOORD1;
    float4 gl_FragCoord : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : SV_Target0;
};

float4 _main(PS_Input Input)
{
    float4 Output = g_colorTexture.Sample(g_colorSampler, Input.UV) * Input.Color;
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
    Input.UV = Input_UV;
    Input.Color = Input_Color;
    float4 _65 = _main(Input);
    _entryPointOutput = _65;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_UV = stage_input.Input_UV;
    Input_Color = stage_input.Input_Color;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
