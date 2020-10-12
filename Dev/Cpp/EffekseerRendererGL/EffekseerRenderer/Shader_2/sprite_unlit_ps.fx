#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec4 Color;
    vec2 UV;
};

struct PS_ConstanBuffer
{
    vec4 flipbookParameter;
    vec4 uvDistortionParameter;
    vec4 blendTextureParameter;
    vec4 emissiveScaling;
    vec4 edgeColor;
    vec4 edgeParameter;
};

uniform PS_ConstanBuffer CBPS0;

uniform sampler2D Sampler_g_sampler;

centroid varying vec4 _VSPS_Color;
centroid varying vec2 _VSPS_UV;

vec4 _main(PS_Input Input)
{
    vec4 Output = Input.Color * texture2D(Sampler_g_sampler, Input.UV);
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
    vec4 _65 = _main(Input);
    gl_FragData[0] = _65;
}

