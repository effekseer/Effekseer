#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif
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
    mat4 mModel[10];
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
centroid out vec2 _VSPS_UV;
out vec3 _VSPS_Normal;
out vec3 _VSPS_Binormal;
out vec3 _VSPS_Tangent;
centroid out vec4 _VSPS_Color;

VS_Output _main(VS_Input Input)
{
    uint index = Input.Index;
    mat4 matModel = CBVS0.mModel[index];
    vec4 uv = CBVS0.fUV[index];
    vec4 modelColor = CBVS0.fModelColor[index] * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec4(0.0));
    vec4 localPosition = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 cameraPosition = localPosition * matModel;
    Output.Pos = cameraPosition * CBVS0.mCameraProj;
    Output.Color = modelColor;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
    vec4 localNormal = vec4(Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0);
    localNormal = normalize(localNormal * matModel);
    vec4 localBinormal = vec4(Input.Binormal.x, Input.Binormal.y, Input.Binormal.z, 0.0);
    localBinormal = normalize(localBinormal * matModel);
    vec4 localTangent = vec4(Input.Tangent.x, Input.Tangent.y, Input.Tangent.z, 0.0);
    localTangent = normalize(localTangent * matModel);
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
    Input.Index = uint((gl_InstanceID + SPIRV_Cross_BaseInstance));
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.Pos;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_Normal = flattenTemp.Normal;
    _VSPS_Binormal = flattenTemp.Binormal;
    _VSPS_Tangent = flattenTemp.Tangent;
    _VSPS_Color = flattenTemp.Color;
}

