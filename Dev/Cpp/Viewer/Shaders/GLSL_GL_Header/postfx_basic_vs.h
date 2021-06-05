static const char gl_postfx_basic_vs[] = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_Input
{
    vec2 Pos;
    vec2 UV;
};

struct VS_Output
{
    vec4 Position;
    vec2 UV;
};

layout(location = 0) in vec2 Input_Pos;
layout(location = 1) in vec2 Input_UV;
out vec2 _VSPS_UV;

VS_Output _main(VS_Input Input)
{
    vec4 pos4 = vec4(Input.Pos.x, Input.Pos.y, 0.0, 1.0);
    VS_Output Output;
    Output.Position = pos4;
    Output.UV = Input.UV;
    Output.UV.y = 1.0 - Output.UV.y;
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.UV = Input_UV;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.Position;
    _VSPS_UV = flattenTemp.UV;
}

)";

