#version 120
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

attribute vec3 Input_Pos;
attribute vec4 Input_Color;
attribute vec4 Input_Normal;
attribute vec4 Input_Tangent;
attribute vec2 Input_UV1;
attribute vec2 Input_UV2;
centroid varying vec4 _VSPS_VColor;
centroid varying vec2 _VSPS_UV1;
centroid varying vec2 _VSPS_UV2;
varying vec3 _VSPS_WorldP;
varying vec3 _VSPS_WorldN;
varying vec3 _VSPS_WorldT;
varying vec3 _VSPS_WorldB;
varying vec2 _VSPS_ScreenUV;

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
    vec4 cameraPos = CBVS0.mCamera * vec4(worldPos, 1.0);
    cameraPos /= vec4(cameraPos.w);
    Output.Position = CBVS0.mProj * cameraPos;
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

