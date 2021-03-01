#version 200 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec4 Color;
    highp vec2 UV;
    highp vec4 PosP;
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
    highp vec4 fFalloffParameter;
    highp vec4 fFalloffBeginColor;
    highp vec4 fFalloffEndColor;
    highp vec4 fEmissiveScaling;
    highp vec4 fEdgeColor;
    highp vec4 fEdgeParameter;
    highp vec4 softParticleParam;
    highp vec4 reconstructionParam1;
    highp vec4 reconstructionParam2;
    highp vec4 mUVInversedBack;
};

uniform PS_ConstanBuffer CBPS0;

uniform highp sampler2D Sampler_sampler_colorTex;

centroid varying highp vec4 _VSPS_Color;
centroid varying highp vec2 _VSPS_UV;
varying highp vec4 _VSPS_PosP;

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = texture2D(Sampler_sampler_colorTex, Input.UV) * Input.Color;
    highp vec3 _45 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_45.x, _45.y, _45.z, Output.w);
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
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    Input.PosP = _VSPS_PosP;
    highp vec4 _83 = _main(Input);
    gl_FragData[0] = _83;
}

