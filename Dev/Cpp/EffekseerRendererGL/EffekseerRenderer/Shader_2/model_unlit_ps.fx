#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 PosVS;
    vec2 UV;
    vec4 Color;
    vec4 PosP;
};

struct VS_ConstantBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 softParticleAndReconstructionParam1;
    vec4 reconstructionParam2;
};

uniform VS_ConstantBuffer CBPS0;

uniform sampler2D Sampler_sampler_colorTex;

centroid varying vec2 _VSPS_UV;
centroid varying vec4 _VSPS_Color;
varying vec4 _VSPS_PosP;

vec4 _main(PS_Input Input)
{
    vec4 Output = texture2D(Sampler_sampler_colorTex, Input.UV) * Input.Color;
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
    vec4 _69 = _main(Input);
    gl_FragData[0] = _69;
}

