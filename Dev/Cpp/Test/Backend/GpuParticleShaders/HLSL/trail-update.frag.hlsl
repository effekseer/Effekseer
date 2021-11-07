struct PS_INPUT
{
    float2 ScreenUV;
};

struct PS_OUTPUT
{
    float4 o_Histories;
};

Texture2D<float4> i_Position : register(t0);
SamplerState i_PositionSampler : register(s0);
Texture2D<float4> i_Velocity : register(t1);
SamplerState i_VelocitySampler : register(s1);

static float2 input_ScreenUV;
static float4 _entryPointOutput_o_Histories;

struct SPIRV_Cross_Input
{
    float2 input_ScreenUV : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput_o_Histories : TEXCOORD0;
};

float packVec3(float3 v)
{
    uint3 i = uint3(((v + 1.0f.xxx) * 0.5f) * 1023.0f);
    return asfloat((i.x | (i.y << uint(10))) | (i.z << uint(20)));
}

PS_OUTPUT _main(PS_INPUT _input)
{
    float4 position = i_Position.SampleLevel(i_PositionSampler, float2(float4(_input.ScreenUV, 0.0f, 0.0f).xy), 0.0f);
    float4 velocity = i_Velocity.SampleLevel(i_VelocitySampler, float2(float4(_input.ScreenUV, 0.0f, 0.0f).xy), 0.0f);
    float3 param = normalize(velocity.xyz);
    PS_OUTPUT _output;
    _output.o_Histories = float4(position.xyz, packVec3(param));
    return _output;
}

void vert_main()
{
    PS_INPUT _input;
    _input.ScreenUV = input_ScreenUV;
    PS_INPUT param = _input;
    _entryPointOutput_o_Histories = _main(param).o_Histories;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    input_ScreenUV = stage_input.input_ScreenUV;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput_o_Histories = _entryPointOutput_o_Histories;
    return stage_output;
}
