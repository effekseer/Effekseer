static const char gl_line_ps[] = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec4 Color;
    vec2 UV;
    vec4 Position;
};

struct PS_Output
{
    vec4 o0;
    vec4 o1;
};

in vec4 _VSPS_Color;
in vec2 _VSPS_UV;
in vec4 _VSPS_Position;
layout(location = 0) out vec4 _entryPointOutput_o0;
layout(location = 1) out vec4 _entryPointOutput_o1;

PS_Output _main(PS_Input Input)
{
    PS_Output _output;
    _output.o0 = Input.Color;
    _output.o1 = vec4(1.0);
    _output.o1.x = Input.Position.z / Input.Position.w;
    if (_output.o0.w == 0.0)
    {
        discard;
    }
    return _output;
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    Input.Position = _VSPS_Position;
    PS_Output _67 = _main(Input);
    PS_Output flattenTemp = _67;
    _entryPointOutput_o0 = flattenTemp.o0;
    _entryPointOutput_o1 = flattenTemp.o1;
}

)";

