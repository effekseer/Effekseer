#version 420

out gl_PerVertex
{
    vec4 gl_Position;
};

struct VS_INPUT
{
    vec2 a_Position;
};

struct VS_OUTPUT
{
    vec4 Position;
};

layout(location = 0) in vec2 input_a_Position;

VS_OUTPUT _main(VS_INPUT _input)
{
    VS_OUTPUT _output;
    _output.Position = vec4(_input.a_Position, 0.0, 1.0);
    return _output;
}

void main()
{
    VS_INPUT _input;
    _input.a_Position = input_a_Position;
    VS_INPUT param = _input;
    gl_Position = _main(param).Position;
}

