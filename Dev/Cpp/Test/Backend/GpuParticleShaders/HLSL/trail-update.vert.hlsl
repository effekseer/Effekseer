struct VS_INPUT
{
    float2 a_Position;
};

struct VS_OUTPUT
{
    float4 Position;
    float2 ScreenUV;
};

static float4 gl_Position;
static float2 input_a_Position;
static float2 _entryPointOutput_ScreenUV;

struct SPIRV_Cross_Input
{
    float2 input_a_Position : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float2 _entryPointOutput_ScreenUV : TEXCOORD0;
    float4 gl_Position : SV_Position;
};

VS_OUTPUT _main(VS_INPUT _input)
{
    VS_OUTPUT _output;
    _output.Position = float4(_input.a_Position, 0.0f, 1.0f);
    _output.ScreenUV = _input.a_Position;
    return _output;
}

void vert_main()
{
    VS_INPUT _input;
    _input.a_Position = input_a_Position;
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    gl_Position = flattenTemp.Position;
    _entryPointOutput_ScreenUV = flattenTemp.ScreenUV;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    input_a_Position = stage_input.input_a_Position;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output._entryPointOutput_ScreenUV = _entryPointOutput_ScreenUV;
    return stage_output;
}
