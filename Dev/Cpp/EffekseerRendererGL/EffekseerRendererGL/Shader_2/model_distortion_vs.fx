#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_Input
{
    vec3 Pos;
    vec2 OctNormal;
    vec2 OctTangent;
    vec2 UV1;
    vec2 UV2;
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
attribute vec2 Input_OctNormal;
attribute vec2 Input_OctTangent;
attribute vec2 Input_UV1;
attribute vec2 Input_UV2;
attribute vec4 Input_Color;
centroid varying vec2 _VSPS_UV;
varying vec4 _VSPS_ProjBinormal;
varying vec4 _VSPS_ProjTangent;
varying vec4 _VSPS_PosP;
centroid varying vec4 _VSPS_Color;

vec3 decodeOct(vec2 oct)
{
    vec3 v = vec3(oct, (1.0 - abs(oct.x)) - abs(oct.y));
    float t = max(-v.z, 0.0);
    vec3 _56 = v;
    vec3 _61 = v;
    vec2 _63 = _61.xy + ((-sign(_56.xy)) * t);
    v.x = _63.x;
    v.y = _63.y;
    return normalize(v);
}

void decodeOct(inout vec2 octNormal, inout vec2 octTangent, inout vec3 normal, out vec3 binormal, inout vec3 tangent)
{
    octNormal = (octNormal * 2.0) - vec2(1.0);
    vec2 param = octNormal;
    normal = decodeOct(param);
    octTangent = (octTangent * 2.0) - vec2(1.0);
    float s = sign(octTangent.y);
    octTangent = vec2(octTangent.x, (abs(octTangent.y) * 2.0) - 1.0);
    vec2 param_1 = octTangent;
    tangent = decodeOct(param_1);
    binormal = cross(normal, tangent) * s;
}

VS_Output _main(VS_Input Input)
{
    vec4 uv = CBVS0.fUV;
    vec4 modelColor = CBVS0.fModelColor * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 localPos = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 worldPos = CBVS0.mModel * localPos;
    Output.PosVS = CBVS0.mCameraProj * worldPos;
    Output.Color = modelColor;
    vec2 outputUV = Input.UV1;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * outputUV.y);
    Output.UV = outputUV;
    vec2 param = Input.OctNormal;
    vec2 param_1 = Input.OctTangent;
    vec3 param_2;
    vec3 param_3;
    vec3 param_4;
    decodeOct(param, param_1, param_2, param_3, param_4);
    vec3 localNormal = param_2;
    vec3 localBinormal = param_3;
    vec3 localTangent = param_4;
    vec4 worldNormal = CBVS0.mModel * vec4(localNormal, 0.0);
    vec4 worldBinormal = CBVS0.mModel * vec4(localTangent, 0.0);
    vec4 worldTangent = CBVS0.mModel * vec4(localBinormal, 0.0);
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
    Input.OctNormal = Input_OctNormal;
    Input.OctTangent = Input_OctTangent;
    Input.UV1 = Input_UV1;
    Input.UV2 = Input_UV2;
    Input.Color = Input_Color;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_ProjBinormal = flattenTemp.ProjBinormal;
    _VSPS_ProjTangent = flattenTemp.ProjTangent;
    _VSPS_PosP = flattenTemp.PosP;
    _VSPS_Color = flattenTemp.Color;
}

