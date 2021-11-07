struct VS_INPUT
{
    float3 a_Particle;
    float3 a_Position;
    float3 a_Direction;
};

struct VS_OUTPUT
{
    float4 Position;
    float3 v_Particle;
    float3 v_Position;
    float3 v_Direction;
};

cbuffer CB : register(b0)
{
    float4 _31_ID2TPos : packoffset(c0);
    float4 _31_TPos2VPos : packoffset(c1);
};


static float4 gl_Position;
static float3 input_a_Particle;
static float3 input_a_Position;
static float3 input_a_Direction;
static float3 _entryPointOutput_v_Particle;
static float3 _entryPointOutput_v_Position;
static float3 _entryPointOutput_v_Direction;

struct SPIRV_Cross_Input
{
    float3 input_a_Particle : TEXCOORD0;
    float3 input_a_Position : TEXCOORD1;
    float3 input_a_Direction : TEXCOORD2;
};

struct SPIRV_Cross_Output
{
    float3 _entryPointOutput_v_Particle : TEXCOORD0;
    float3 _entryPointOutput_v_Position : TEXCOORD1;
    float3 _entryPointOutput_v_Direction : TEXCOORD2;
    float4 gl_Position : SV_Position;
};

VS_OUTPUT _main(VS_INPUT _input)
{
    int particleID = int(_input.a_Particle.x);
    int2 ID2TPos2i = int2(int(_31_ID2TPos.x), int(_31_ID2TPos.y));
    float2 glpos = (float2(float(particleID & ID2TPos2i.x), float(particleID >> ID2TPos2i.y)) * _31_TPos2VPos.xy) + _31_TPos2VPos.zw;
    VS_OUTPUT _output;
    _output.Position = float4(glpos, 0.0f, 1.0f);
    _output.v_Particle = _input.a_Particle;
    _output.v_Position = _input.a_Position;
    _output.v_Direction = _input.a_Direction;
    return _output;
}

void vert_main()
{
    VS_INPUT _input;
    _input.a_Particle = input_a_Particle;
    _input.a_Position = input_a_Position;
    _input.a_Direction = input_a_Direction;
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    gl_Position = flattenTemp.Position;
    _entryPointOutput_v_Particle = flattenTemp.v_Particle;
    _entryPointOutput_v_Position = flattenTemp.v_Position;
    _entryPointOutput_v_Direction = flattenTemp.v_Direction;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    input_a_Particle = stage_input.input_a_Particle;
    input_a_Position = stage_input.input_a_Position;
    input_a_Direction = stage_input.input_a_Direction;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output._entryPointOutput_v_Particle = _entryPointOutput_v_Particle;
    stage_output._entryPointOutput_v_Position = _entryPointOutput_v_Position;
    stage_output._entryPointOutput_v_Direction = _entryPointOutput_v_Direction;
    return stage_output;
}
