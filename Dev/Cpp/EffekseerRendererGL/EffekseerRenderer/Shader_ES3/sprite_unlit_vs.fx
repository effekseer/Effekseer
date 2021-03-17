#version 300 es

struct VS_Input
{
    vec3 Pos;
    vec4 Color;
    vec2 UV;
};

struct VS_Output
{
    vec4 PosVS;
    vec4 Color;
    vec2 UV;
    vec4 PosP;
};

struct VS_ConstantBuffer
{
    mat4 mCamera;
    mat4 mCameraProj;
    vec4 mUVInversed;
    vec4 mflipbookParameter;
};

uniform VS_ConstantBuffer CBVS0;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec4 Input_Color;
layout(location = 2) in vec2 Input_UV;
centroid out vec4 _VSPS_Color;
centroid out vec2 _VSPS_UV;
out vec4 _VSPS_PosP;

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec2(0.0), vec4(0.0));
    vec4 worldPos = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    Output.PosVS = worldPos * CBVS0.mCameraProj;
    Output.Color = Input.Color;
    vec2 uv1 = Input.UV;
    uv1.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * uv1.y);
    Output.UV = uv1;
    Output.PosP = Output.PosVS;
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _VSPS_Color = flattenTemp.Color;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_PosP = flattenTemp.PosP;
}

