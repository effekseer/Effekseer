#version 420

out gl_PerVertex
{
    vec4 gl_Position;
};

struct VS_INPUT
{
    vec3 a_Particle;
    vec3 a_Position;
    vec3 a_Direction;
};

struct VS_OUTPUT
{
    vec4 Position;
    vec3 v_Particle;
    vec3 v_Position;
    vec3 v_Direction;
};

layout(binding = 0, std140) uniform CB
{
    vec4 ID2TPos;
    vec4 TPos2VPos;
} CBVS0;

layout(location = 0) in vec3 input_a_Particle;
layout(location = 1) in vec3 input_a_Position;
layout(location = 2) in vec3 input_a_Direction;
layout(location = 0) out vec3 _entryPointOutput_v_Particle;
layout(location = 1) out vec3 _entryPointOutput_v_Position;
layout(location = 2) out vec3 _entryPointOutput_v_Direction;

VS_OUTPUT _main(VS_INPUT _input)
{
    int particleID = int(_input.a_Particle.x);
    ivec2 ID2TPos2i = ivec2(int(CBVS0.ID2TPos.x), int(CBVS0.ID2TPos.y));
    vec2 glpos = (vec2(float(particleID & ID2TPos2i.x), float(particleID >> ID2TPos2i.y)) * CBVS0.TPos2VPos.xy) + CBVS0.TPos2VPos.zw;
    VS_OUTPUT _output;
    _output.Position = vec4(glpos, 0.0, 1.0);
    _output.v_Particle = _input.a_Particle;
    _output.v_Position = _input.a_Position;
    _output.v_Direction = _input.a_Direction;
    return _output;
}

void main()
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

