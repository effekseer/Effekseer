#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 PosVS;
    vec4 Color;
    vec2 UV;
    vec4 PosP;
    vec4 PosU;
    vec4 PosR;
};

struct VS_ConstantBuffer
{
    vec4 g_scale;
    vec4 mUVInversedBack;
    vec4 flipbookParameter;
    vec4 uvDistortionParameter;
    vec4 blendTextureParameter;
    vec4 softParticleAndReconstructionParam1;
    vec4 reconstructionParam2;
};

uniform VS_ConstantBuffer CBPS0;

uniform sampler2D Sampler_sampler_colorTex;
uniform sampler2D Sampler_sampler_backTex;

centroid varying vec4 _VSPS_Color;
centroid varying vec2 _VSPS_UV;
varying vec4 _VSPS_PosP;
varying vec4 _VSPS_PosU;
varying vec4 _VSPS_PosR;

vec4 _main(PS_Input Input)
{
    vec4 Output = texture2D(Sampler_sampler_colorTex, Input.UV);
    Output.w *= Input.Color.w;
    vec2 pos = Input.PosP.xy / vec2(Input.PosP.w);
    vec2 posU = Input.PosU.xy / vec2(Input.PosU.w);
    vec2 posR = Input.PosR.xy / vec2(Input.PosR.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * CBPS0.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * CBPS0.g_scale.x;
    vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * uv.y);
    uv.y = 1.0 - uv.y;
    vec3 color = vec3(texture2D(Sampler_sampler_backTex, uv).xyz);
    Output = vec4(color.x, color.y, color.z, Output.w);
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
    Input.PosU = _VSPS_PosU;
    Input.PosR = _VSPS_PosR;
    vec4 _182 = _main(Input);
    gl_FragData[0] = _182;
}

