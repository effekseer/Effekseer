#version 200 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec2 UV;
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

centroid varying highp vec2 _VSPS_UV;
centroid varying highp vec4 _VSPS_Color;
varying highp vec4 _VSPS_PosP;

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = texture2D(Sampler_sampler_colorTex, Input.UV) * Input.Color;
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
    Input.Color = _VSPS_Color;
    Input.PosP = _VSPS_PosP;
    highp vec4 _69 = _main(Input);
    gl_FragData[0] = _69;
}

