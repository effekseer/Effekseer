#version 300 es
#ifdef GL_ARB_shader_draw_parameters
#extension GL_ARB_shader_draw_parameters : enable
#endif

struct VS_Input
{
    vec3 Pos;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec2 UV;
    vec4 Color;
    uint Index;
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
    mat4 mModel_Inst[10];
    vec4 fUV[10];
    vec4 fModelColor[10];
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 mUVInversed;
};

uniform VS_ConstantBuffer CBVS0;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec3 Input_Normal;
layout(location = 2) in vec3 Input_Binormal;
layout(location = 3) in vec3 Input_Tangent;
layout(location = 4) in vec2 Input_UV;
layout(location = 5) in vec4 Input_Color;
#ifdef GL_ARB_shader_draw_parameters
#define SPIRV_Cross_BaseInstance gl_BaseInstanceARB
#else
uniform int SPIRV_Cross_BaseInstance;
#endif
centroid out vec4 _VSPS_Color;
centroid out vec2 _VSPS_UV;
out vec3 _VSPS_WorldN;
out vec3 _VSPS_WorldB;
out vec3 _VSPS_WorldT;
out vec4 _VSPS_PosP;

highp mat4 spvWorkaroundRowMajor(highp mat4 wrap) { return wrap; }
mediump mat4 spvWorkaroundRowMajorMP(mediump mat4 wrap) { return wrap; }

VS_Output _main(VS_Input Input)
{
    uint index = Input.Index;
    mat4 mModel = spvWorkaroundRowMajor(CBVS0.mModel_Inst[index]);
    vec4 uv = CBVS0.fUV[index];
    vec4 modelColor = CBVS0.fModelColor[index] * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec2(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec4(0.0));
    vec4 localPos = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 worldPos = localPos * mModel;
    Output.PosVS = worldPos * spvWorkaroundRowMajor(CBVS0.mCameraProj);
    Output.Color = modelColor;
    vec2 outputUV = Input.UV;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * outputUV.y);
    Output.UV = outputUV;
    vec4 localNormal = vec4(Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0);
    vec4 localBinormal = vec4(Input.Binormal.x, Input.Binormal.y, Input.Binormal.z, 0.0);
    vec4 localTangent = vec4(Input.Tangent.x, Input.Tangent.y, Input.Tangent.z, 0.0);
    vec4 worldNormal = localNormal * mModel;
    vec4 worldBinormal = localBinormal * mModel;
    vec4 worldTangent = localTangent * mModel;
    worldNormal = normalize(worldNormal);
    worldBinormal = normalize(worldBinormal);
    worldTangent = normalize(worldTangent);
    Output.WorldN = worldNormal.xyz;
    Output.WorldB = worldBinormal.xyz;
    Output.WorldT = worldTangent.xyz;
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
    Input.Index = uint((gl_InstanceID + SPIRV_Cross_BaseInstance));
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _VSPS_Color = flattenTemp.Color;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_WorldN = flattenTemp.WorldN;
    _VSPS_WorldB = flattenTemp.WorldB;
    _VSPS_WorldT = flattenTemp.WorldT;
    _VSPS_PosP = flattenTemp.PosP;
}

