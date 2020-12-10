#version 200 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec2 UV;
    highp vec3 Normal;
    highp vec3 Binormal;
    highp vec3 Tangent;
    highp vec4 Color;
    highp vec4 PosP;
};

struct FalloffParameter
{
    highp vec4 Param;
    highp vec4 BeginColor;
    highp vec4 EndColor;
};

struct PS_ConstanBuffer
{
    highp vec4 fLightDirection;
    highp vec4 fLightColor;
    highp vec4 fLightAmbient;
    highp vec4 fFlipbookParameter;
    highp vec4 fUVDistortionParameter;
    highp vec4 fBlendTextureParameter;
    highp vec4 fCameraFrontDirection;
    FalloffParameter fFalloffParam;
    highp vec4 fEmissiveScaling;
    highp vec4 fEdgeColor;
    highp vec4 fEdgeParameter;
    highp vec4 softParticleAndReconstructionParam1;
    highp vec4 reconstructionParam2;
};

uniform PS_ConstanBuffer CBPS0;

uniform highp sampler2D Sampler_sampler_colorTex;
uniform highp sampler2D Sampler_sampler_normalTex;

centroid varying highp vec2 _VSPS_UV;
varying highp vec3 _VSPS_Normal;
varying highp vec3 _VSPS_Binormal;
varying highp vec3 _VSPS_Tangent;
centroid varying highp vec4 _VSPS_Color;
varying highp vec4 _VSPS_PosP;

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = texture2D(Sampler_sampler_colorTex, Input.UV) * Input.Color;
    highp vec3 texNormal = (texture2D(Sampler_sampler_normalTex, Input.UV).xyz - vec3(0.5)) * 2.0;
    highp vec3 localNormal = normalize(mat3(vec3(Input.Tangent), vec3(Input.Binormal), vec3(Input.Normal)) * texNormal);
    highp float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    highp vec3 _100 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + CBPS0.fLightAmbient.xyz);
    Output = vec4(_100.x, _100.y, _100.z, Output.w);
    if (Output.w == 0.0)
    {
        discard;
    }
    return Output;
}

void main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.UV = _VSPS_UV;
    Input.Normal = _VSPS_Normal;
    Input.Binormal = _VSPS_Binormal;
    Input.Tangent = _VSPS_Tangent;
    Input.Color = _VSPS_Color;
    Input.PosP = _VSPS_PosP;
    highp vec4 _146 = _main(Input);
    gl_FragData[0] = _146;
}

