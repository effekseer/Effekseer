struct PS_Input
{
    float4 PosVS;
    float2 UV;
    float4 Color;
    float4 PosP;
};

cbuffer VS_ConstantBuffer : register(b0)
{
    float4 _72_fLightDirection : register(c0);
    float4 _72_fLightColor : register(c1);
    float4 _72_fLightAmbient : register(c2);
    float4 _72_softParticleAndReconstructionParam1 : register(c3);
    float4 _72_reconstructionParam2 : register(c4);
};

uniform sampler2D Sampler_sampler_colorTex : register(s0);

static float4 gl_FragCoord;
static float2 Input_UV;
static float4 Input_Color;
static float4 Input_PosP;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float2 Input_UV : TEXCOORD0;
    centroid float4 Input_Color : TEXCOORD1;
    float4 Input_PosP : TEXCOORD2;
    float4 gl_FragCoord : VPOS;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : COLOR0;
};

float4 _main(PS_Input Input)
{
    float4 Output = tex2D(Sampler_sampler_colorTex, Input.UV) * Input.Color;
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
    Input.Color = Input_Color;
    Input.PosP = Input_PosP;
    float4 _69 = _main(Input);
    _entryPointOutput = _69;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord + float4(0.5f, 0.5f, 0.0f, 0.0f);
    Input_UV = stage_input.Input_UV;
    Input_Color = stage_input.Input_Color;
    Input_PosP = stage_input.Input_PosP;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = float4(_entryPointOutput);
    return stage_output;
}
