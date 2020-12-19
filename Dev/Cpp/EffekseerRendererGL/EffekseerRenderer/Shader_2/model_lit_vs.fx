#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_Input
{
    vec3 Pos;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec2 UV;
    vec4 Color;
};

struct VS_Output
{
    vec4 PosVS;
    vec4 Color;
    vec2 UV;
    vec3 WorldN;
    vec3 WorldB;
    vec3 WorldT;
    vec4 PosP;
};

struct VS_ConstantBuffer
{
    mat4 mCameraProj;
    mat4 mModel;
    vec4 fUV;
    vec4 fModelColor;
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 mUVInversed;
};

uniform VS_ConstantBuffer CBVS0;

attribute vec3 Input_Pos;
attribute vec3 Input_Normal;
attribute vec3 Input_Binormal;
attribute vec3 Input_Tangent;
attribute vec2 Input_UV;
attribute vec4 Input_Color;
centroid varying vec4 _VSPS_Color;
centroid varying vec2 _VSPS_UV;
varying vec3 _VSPS_WorldN;
varying vec3 _VSPS_WorldB;
varying vec3 _VSPS_WorldT;
varying vec4 _VSPS_PosP;

VS_Output _main(VS_Input Input)
{
    vec4 uv = CBVS0.fUV;
    vec4 modelColor = CBVS0.fModelColor * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec2(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec4(0.0));
    vec4 localPosition = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    localPosition = CBVS0.mModel * localPosition;
    Output.PosVS = CBVS0.mCameraProj * localPosition;
    Output.Color = modelColor;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
    vec4 localNormal = vec4(Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0);
    vec4 localBinormal = vec4(Input.Binormal.x, Input.Binormal.y, Input.Binormal.z, 0.0);
    vec4 localTangent = vec4(Input.Tangent.x, Input.Tangent.y, Input.Tangent.z, 0.0);
    localNormal = CBVS0.mModel * localNormal;
    localBinormal = CBVS0.mModel * localBinormal;
    localTangent = CBVS0.mModel * localTangent;
    localNormal = normalize(localNormal);
    localBinormal = normalize(localBinormal);
    localTangent = normalize(localTangent);
    Output.WorldN = localNormal.xyz;
    Output.WorldB = localBinormal.xyz;
    Output.WorldT = localTangent.xyz;
    Output.UV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * Output.UV.y);
    Output.PosP = Output.PosVS;
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.UV = Input_UV;
    Input.Color = Input_Color;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _VSPS_Color = flattenTemp.Color;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_WorldN = flattenTemp.WorldN;
    _VSPS_WorldB = flattenTemp.WorldB;
    _VSPS_WorldT = flattenTemp.WorldT;
    _VSPS_PosP = flattenTemp.PosP;
}

