#version 420

struct PS_INPUT
{
    vec3 v_Particle;
    vec3 v_Position;
    vec3 v_Direction;
};

struct PS_OUTPUT
{
    vec4 o_ParticleData0;
    vec4 o_ParticleData1;
};

layout(location = 0) in vec3 input_v_Particle;
layout(location = 1) in vec3 input_v_Position;
layout(location = 2) in vec3 input_v_Direction;
layout(location = 0) out vec4 _entryPointOutput_o_ParticleData0;
layout(location = 1) out vec4 _entryPointOutput_o_ParticleData1;

float packVec3(vec3 v)
{
    uvec3 i = uvec3(((v + vec3(1.0)) * 0.5) * 1023.0);
    return uintBitsToFloat((i.x | (i.y << uint(10))) | (i.z << uint(20)));
}

PS_OUTPUT _main(PS_INPUT _input)
{
    vec3 param = normalize(_input.v_Direction);
    float packedDir = packVec3(param);
    PS_OUTPUT _output;
    _output.o_ParticleData0 = vec4(_input.v_Position, packedDir);
    _output.o_ParticleData1 = vec4(0.0, _input.v_Particle.y, _input.v_Particle.z, packedDir);
    return _output;
}

void main()
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

