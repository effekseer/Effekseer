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
    vec2 UV;
    vec4 ProjBinormal;
    vec4 ProjTangent;
    vec4 PosP;
    vec4 Color;
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
centroid varying vec2 _VSPS_UV;
varying vec4 _VSPS_ProjBinormal;
varying vec4 _VSPS_ProjTangent;
varying vec4 _VSPS_PosP;
centroid varying vec4 _VSPS_Color;

VS_Output _main(VS_Input Input)
{
    vec4 uv = CBVS0.fUV;
    vec4 modelColor = CBVS0.fModelColor * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 localPos = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 worldPos = CBVS0.mModel * localPos;
    Output.PosVS = CBVS0.mCameraProj * worldPos;
    Output.Color = modelColor;
    vec2 outputUV = Input.UV;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * outputUV.y);
    Output.UV = outputUV;
    vec4 localNormal = vec4(Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0);
    vec4 localBinormal = vec4(Input.Binormal.x, Input.Binormal.y, Input.Binormal.z, 0.0);
    vec4 localTangent = vec4(Input.Tangent.x, Input.Tangent.y, Input.Tangent.z, 0.0);
    vec4 worldNormal = CBVS0.mModel * localNormal;
    vec4 worldBinormal = CBVS0.mModel * localBinormal;
    vec4 worldTangent = CBVS0.mModel * localTangent;
    worldNormal = normalize(worldNormal);
    worldBinormal = normalize(worldBinormal);
    worldTangent = normalize(worldTangent);
    Output.ProjBinormal = CBVS0.mCameraProj * (worldPos + worldBinormal);
    Output.ProjTangent = CBVS0.mCameraProj * (worldPos + worldTangent);
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
    _VSPS_UV = flattenTemp.UV;
    _VSPS_ProjBinormal = flattenTemp.ProjBinormal;
    _VSPS_ProjTangent = flattenTemp.ProjTangent;
    _VSPS_PosP = flattenTemp.PosP;
    _VSPS_Color = flattenTemp.Color;
}

