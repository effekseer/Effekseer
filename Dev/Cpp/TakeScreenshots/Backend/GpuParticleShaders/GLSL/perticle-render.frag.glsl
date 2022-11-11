#version 420

struct PS_INPUT
{
    vec3 v_Color;
};

struct PS_OUTPUT
{
    vec4 o_FragColor;
};

layout(location = 0) in vec3 input_v_Color;
layout(location = 0) out vec4 _entryPointOutput_o_FragColor;

PS_OUTPUT _main(PS_INPUT _input)
{
    PS_OUTPUT _output;
    _output.o_FragColor = vec4(_input.v_Color, 1.0);
    return _output;
}

void main()
{
    PS_INPUT _input;
    _input.v_Color = input_v_Color;
    PS_INPUT param = _input;
    _entryPointOutput_o_FragColor = _main(param).o_FragColor;
}

