#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_Input
{
    vec3 Pos;
    vec4 Color;
    vec4 Normal;
    vec4 Tangent;
    vec2 UV1;
    vec2 UV2;
};

struct VS_Output
{
    vec4 Position;
    vec4 VColor;
    vec2 UV1;
    vec2 UV2;
    vec3 WorldP;
    vec3 WorldN;
    vec3 WorldT;
    vec3 WorldB;
    vec2 ScreenUV;
};

struct VS_ConstantBuffer
{
    mat4 mCamera;
    mat4 mProj;
    vec4 mUVInversed;
    vec4 mflipbookParameter;
};

uniform VS_ConstantBuffer CBVS0;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec4 Input_Color;
layout(location = 2) in vec4 Input_Normal;
layout(location = 3) in vec4 Input_Tangent;
layout(location = 4) in vec2 Input_UV1;
layout(location = 5) in vec2 Input_UV2;
out vec4 _VSPS_VColor;
out vec2 _VSPS_UV1;
out vec2 _VSPS_UV2;
out vec3 _VSPS_WorldP;
out vec3 _VSPS_WorldN;
out vec3 _VSPS_WorldT;
out vec3 _VSPS_WorldB;
out vec2 _VSPS_ScreenUV;

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec2(0.0), vec2(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec2(0.0));
    vec3 worldPos = Input.Pos;
    vec3 worldNormal = (vec3(Input.Normal.xyz) - vec3(0.5)) * 2.0;
    vec3 worldTangent = (vec3(Input.Tangent.xyz) - vec3(0.5)) * 2.0;
    vec3 worldBinormal = cross(worldNormal, worldTangent);
    vec2 uv1 = Input.UV1;
    vec2 uv2 = Input.UV1;
    uv1.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * uv1.y);
    uv2.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * uv2.y);
    Output.WorldN = worldNormal;
    Output.WorldB = worldBinormal;
    Output.WorldT = worldTangent;
    vec3 pixelNormalDir = vec3(0.5, 0.5, 1.0);
    vec4 cameraPos = vec4(worldPos, 1.0) * CBVS0.mCamera;
    cameraPos /= vec4(cameraPos.w);
    Output.Position = cameraPos * CBVS0.mProj;
    Output.WorldP = worldPos;
    Output.VColor = Input.Color;
    Output.UV1 = uv1;
    Output.UV2 = uv2;
    Output.ScreenUV = Output.Position.xy / vec2(Output.Position.w);
    Output.ScreenUV = vec2(Output.ScreenUV.x + 1.0, 1.0 - Output.ScreenUV.y) * 0.5;
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.Normal = Input_Normal;
    Input.Tangent = Input_Tangent;
    Input.UV1 = Input_UV1;
    Input.UV2 = Input_UV2;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.Position;
    _VSPS_VColor = flattenTemp.VColor;
    _VSPS_UV1 = flattenTemp.UV1;
    _VSPS_UV2 = flattenTemp.UV2;
    _VSPS_WorldP = flattenTemp.WorldP;
    _VSPS_WorldN = flattenTemp.WorldN;
    _VSPS_WorldT = flattenTemp.WorldT;
    _VSPS_WorldB = flattenTemp.WorldB;
    _VSPS_ScreenUV = flattenTemp.ScreenUV;
}

