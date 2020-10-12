#version 200 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 Pos;
    highp vec4 Color;
    highp vec2 UV;
};

struct PS_ConstanBuffer
{
    highp vec4 flipbookParameter;
    highp vec4 uvDistortionParameter;
    highp vec4 blendTextureParameter;
    highp vec4 emissiveScaling;
    highp vec4 edgeColor;
    highp vec4 edgeParameter;
};

uniform PS_ConstanBuffer CBPS0;

uniform highp sampler2D Sampler_g_sampler;

centroid varying highp vec4 _VSPS_Color;
centroid varying highp vec2 _VSPS_UV;

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = Input.Color * texture2D(Sampler_g_sampler, Input.UV);
    if (Output.w == 0.0)
    {
        discard;
    }
    return Output;
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    highp vec4 _65 = _main(Input);
    gl_FragData[0] = _65;
}

