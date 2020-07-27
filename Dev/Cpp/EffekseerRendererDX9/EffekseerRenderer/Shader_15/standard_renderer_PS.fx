struct PS_Input
{
    float4 Pos;
    float4 Color;
    float2 UV;
};

uniform sampler2D Sampler_g_sampler : register(s0);

static float4 gl_FragCoord;
static float4 Input_Color;
static float2 Input_UV;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    float4 Input_Color : TEXCOORD0;
    float2 Input_UV : TEXCOORD1;
    float4 gl_FragCoord : VPOS;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : COLOR0;
};

float4 _main(PS_Input Input)
{
    float4 Output = Input.Color * tex2D(Sampler_g_sampler, Input.UV);
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
    gl_FragCoord = stage_input.gl_FragCoord + float4(0.5f, 0.5f, 0.0f, 0.0f);
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = float4(_entryPointOutput);
    return stage_output;
}
