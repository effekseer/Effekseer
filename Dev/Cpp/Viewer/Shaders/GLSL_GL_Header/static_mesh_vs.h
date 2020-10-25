static const char gl_static_mesh_vs[] = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_Input
{
    vec3 Pos;
    vec2 UV;
    vec4 Color;
    vec3 Normal;
};

struct VS_Output
{
    vec4 Pos;
    vec2 UV;
    vec4 Color;
    vec3 Normal;
};

struct VS_ConstantBuffer
{
    mat4 projectionMatrix;
    mat4 cameraMatrix;
};

uniform VS_ConstantBuffer CBVS0;

layout(location = 0) in vec3 input_Pos;
layout(location = 1) in vec2 input_UV;
layout(location = 2) in vec4 input_Color;
layout(location = 3) in vec3 input_Normal;
out vec2 _VSPS_UV;
out vec4 _VSPS_Color;
out vec3 _VSPS_Normal;

VS_Output _main(VS_Input _input)
{
    vec4 localPos = vec4(_input.Pos, 1.0);
    vec4 cameraPos = localPos * CBVS0.cameraMatrix;
    VS_Output _output;
    _output.Pos = cameraPos * CBVS0.projectionMatrix;
    _output.UV = _input.UV;
    _output.Color = _input.Color;
    _output.Normal = _input.Normal;
    return _output;
}

void main()
{
    VS_Input _input;
    _input.Pos = input_Pos;
    _input.UV = input_UV;
    _input.Color = input_Color;
    _input.Normal = input_Normal;
    VS_Output flattenTemp = _main(_input);
    gl_Position = flattenTemp.Pos;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_Color = flattenTemp.Color;
    _VSPS_Normal = flattenTemp.Normal;
}

)";

