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
    vec4 Pos;
    vec2 UV;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
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
varying vec2 _VSPS_UV;
varying vec3 _VSPS_Normal;
varying vec3 _VSPS_Binormal;
varying vec3 _VSPS_Tangent;
varying vec4 _VSPS_Color;

VS_Output _main(VS_Input Input)
{
    mat4 matModel = transpose(CBVS0.mModel);
    vec4 uv = CBVS0.fUV;
    vec4 modelColor = CBVS0.fModelColor * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec4(0.0));
    vec4 localPosition = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    localPosition *= matModel;
    Output.Pos = CBVS0.mCameraProj * localPosition;
    Output.Color = modelColor;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
    mat3 lightMat = mat3(matModel[0].xyz, matModel[1].xyz, matModel[2].xyz);
    vec4 localNormal = vec4(0.0, 0.0, 0.0, 1.0);
    vec3 _103 = normalize(Input.Normal * lightMat);
    localNormal = vec4(_103.x, _103.y, _103.z, localNormal.w);
    vec4 localBinormal = vec4(0.0, 0.0, 0.0, 1.0);
    vec3 _110 = normalize(Input.Binormal * lightMat);
    localBinormal = vec4(_110.x, _110.y, _110.z, localBinormal.w);
    vec4 localTangent = vec4(0.0, 0.0, 0.0, 1.0);
    vec3 _117 = normalize(Input.Tangent * lightMat);
    localTangent = vec4(_117.x, _117.y, _117.z, localTangent.w);
    Output.Normal = localNormal.xyz;
    Output.Binormal = localBinormal.xyz;
    Output.Tangent = localTangent.xyz;
    Output.UV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * Output.UV.y);
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
    gl_Position = flattenTemp.Pos;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_Normal = flattenTemp.Normal;
    _VSPS_Binormal = flattenTemp.Binormal;
    _VSPS_Tangent = flattenTemp.Tangent;
    _VSPS_Color = flattenTemp.Color;
}

