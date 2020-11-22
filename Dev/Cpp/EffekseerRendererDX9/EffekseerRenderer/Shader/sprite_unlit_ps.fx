struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float2 UV;
    float4 PosP;
};

cbuffer PS_ConstanBuffer : register(b0)
{
    float4 _72_flipbookParameter : register(c0);
    float4 _72_uvDistortionParameter : register(c1);
    float4 _72_blendTextureParameter : register(c2);
    float4 _72_emissiveScaling : register(c3);
    float4 _72_edgeColor : register(c4);
    float4 _72_edgeParameter : register(c5);
    float4 _72_softParticleAndReconstructionParam1 : register(c6);
    float4 _72_reconstructionParam2 : register(c7);
};

uniform sampler2D Sampler_sampler_colorTex : register(s0);

static float4 gl_FragCoord;
static float4 Input_Color;
static float2 Input_UV;
static float4 Input_PosP;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float2 Input_UV : TEXCOORD1;
    float4 Input_PosP : TEXCOORD2;
    float4 gl_FragCoord : VPOS;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : COLOR0;
};

float4 _main(PS_Input Input)
{
    float4 Output = Input.Color * tex2D(Sampler_sampler_colorTex, Input.UV);
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
    float4 _69 = _main(Input);
    _entryPointOutput = _69;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord + float4(0.5f, 0.5f, 0.0f, 0.0f);
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    Input_PosP = stage_input.Input_PosP;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = float4(_entryPointOutput);
    return stage_output;
}
