struct PS_INPUT
{
    float3 v_Particle;
    float3 v_Position;
    float3 v_Direction;
};

struct PS_OUTPUT
{
    float4 o_ParticleData0;
    float4 o_ParticleData1;
};

static float3 input_v_Particle;
static float3 input_v_Position;
static float3 input_v_Direction;
static float4 _entryPointOutput_o_ParticleData0;
static float4 _entryPointOutput_o_ParticleData1;

struct SPIRV_Cross_Input
{
    float3 input_v_Particle : TEXCOORD0;
    float3 input_v_Position : TEXCOORD1;
    float3 input_v_Direction : TEXCOORD2;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput_o_ParticleData0 : TEXCOORD0;
    float4 _entryPointOutput_o_ParticleData1 : TEXCOORD1;
};

float packVec3(float3 v)
{
    uint3 i = uint3(((v + 1.0f.xxx) * 0.5f) * 1023.0f);
    return asfloat((i.x | (i.y << uint(10))) | (i.z << uint(20)));
}

PS_OUTPUT _main(PS_INPUT _input)
{
    float3 param = normalize(_input.v_Direction);
    float packedDir = packVec3(param);
    PS_OUTPUT _output;
    _output.o_ParticleData0 = float4(_input.v_Position, packedDir);
    _output.o_ParticleData1 = float4(0.0f, _input.v_Particle.y, _input.v_Particle.z, packedDir);
    return _output;
}

void vert_main()
{
    PS_INPUT _input;
    _input.v_Particle = input_v_Particle;
    _input.v_Position = input_v_Position;
    _input.v_Direction = input_v_Direction;
    PS_INPUT param = _input;
    PS_OUTPUT flattenTemp = _main(param);
    _entryPointOutput_o_ParticleData0 = flattenTemp.o_ParticleData0;
    _entryPointOutput_o_ParticleData1 = flattenTemp.o_ParticleData1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    input_v_Particle = stage_input.input_v_Particle;
    input_v_Position = stage_input.input_v_Position;
    input_v_Direction = stage_input.input_v_Direction;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput_o_ParticleData0 = _entryPointOutput_o_ParticleData0;
    stage_output._entryPointOutput_o_ParticleData1 = _entryPointOutput_o_ParticleData1;
    return stage_output;
}
