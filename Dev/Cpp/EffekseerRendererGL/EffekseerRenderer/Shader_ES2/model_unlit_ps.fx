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

struct VS_ConstantBuffer
{
    highp vec4 fLightDirection;
    highp vec4 fLightColor;
    highp vec4 fLightAmbient;
    highp vec4 softParticleAndReconstructionParam1;
    highp vec4 reconstructionParam2;
};

uniform VS_ConstantBuffer CBPS0;

uniform highp sampler2D Sampler_g_colorSampler;

centroid varying highp vec2 _VSPS_UV;
centroid varying highp vec4 _VSPS_Color;
varying highp vec4 _VSPS_PosP;

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = texture2D(Sampler_g_colorSampler, Input.UV) * Input.Color;
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

