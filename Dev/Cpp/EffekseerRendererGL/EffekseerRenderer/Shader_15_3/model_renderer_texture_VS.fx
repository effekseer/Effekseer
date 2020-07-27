#version 330
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

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec3 Input_Normal;
layout(location = 2) in vec3 Input_Binormal;
layout(location = 3) in vec3 Input_Tangent;
layout(location = 4) in vec2 Input_UV;
layout(location = 5) in vec4 Input_Color;
out vec2 _VSPS_UV;
out vec4 _VSPS_Color;

VS_Output _main(VS_Input Input)
{
    mat4 matModel = CBVS0.mModel;
    vec4 uv = CBVS0.fUV;
    vec4 modelColor = CBVS0.fModelColor * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0));
    vec4 localPosition = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    localPosition *= matModel;
    Output.Pos = localPosition * CBVS0.mCameraProj;
    Output.Color = modelColor;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
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
    _VSPS_Color = flattenTemp.Color;
}

