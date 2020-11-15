static const char gl_line_vs[] = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_Input
{
    vec3 Pos;
    vec4 Color;
    vec2 UV;
};

struct VS_Output
{
    vec4 Pos;
    vec4 Color;
    vec2 UV;
    vec4 Position;
};

struct _Global
{
    mat4 mCamera;
    mat4 mProj;
};

uniform _Global CBVS0;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec4 Input_Color;
layout(location = 2) in vec2 Input_UV;
out vec4 _VSPS_Color;
out vec2 _VSPS_UV;
out vec4 _VSPS_Position;

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec2(0.0), vec4(0.0));
    vec4 pos4 = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 cameraPos = pos4 * CBVS0.mCamera;
    cameraPos /= vec4(cameraPos.w);
    Output.Position = cameraPos * CBVS0.mProj;
    Output.Pos = Output.Position;
    Output.UV = Input.UV;
    Output.Color = Input.Color;
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.Pos;
    _VSPS_Color = flattenTemp.Color;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_Position = flattenTemp.Position;
}

)";

