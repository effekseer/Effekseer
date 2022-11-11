static const char gl_mesh_vs[] = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_Input
{
    vec3 Pos;
    vec2 UV;
    vec4 Color;
};

struct VS_Output
{
    vec4 Pos;
    vec2 UV;
    vec4 Color;
};

struct _Global
{
    vec4 shift_vertex;
};

uniform _Global CBVS0;

layout(location = 0) in vec3 input_Pos;
layout(location = 1) in vec2 input_UV;
layout(location = 2) in vec4 input_Color;
out vec2 _VSPS_UV;
out vec4 _VSPS_Color;

VS_Output _main(VS_Input _input)
{
    VS_Output _output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0));
    _output.Pos = vec4(_input.Pos.x, _input.Pos.y, _input.Pos.z, 1.0) + CBVS0.shift_vertex;
    _output.UV = _input.UV;
    _output.Color = _input.Color;
    return _output;
}

void main()
{
    VS_Input _input;
    _input.Pos = input_Pos;
    _input.UV = input_UV;
    _input.Color = input_Color;
    VS_Output flattenTemp = _main(_input);
    gl_Position = flattenTemp.Pos;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_Color = flattenTemp.Color;
}

)";

