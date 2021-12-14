#version 200 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec4 Color;
    highp vec2 UV;
    highp vec3 WorldN;
    highp vec3 WorldB;
    highp vec3 WorldT;
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
    highp vec4 miscFlags;
};

uniform PS_ConstanBuffer CBPS0;

uniform highp sampler2D Sampler_sampler_colorTex;
uniform highp sampler2D Sampler_sampler_normalTex;

centroid varying highp vec4 _VSPS_Color;
centroid varying highp vec2 _VSPS_UV;
varying highp vec3 _VSPS_WorldN;
varying highp vec3 _VSPS_WorldB;
varying highp vec3 _VSPS_WorldT;
varying highp vec4 _VSPS_PosP;

highp vec3 PositivePow(highp vec3 base, highp vec3 power)
{
    return pow(max(abs(base), vec3(1.1920928955078125e-07)), power);
}

highp vec3 LinearToSRGB(highp vec3 c)
{
    highp vec3 param = c;
    highp vec3 param_1 = vec3(0.4166666567325592041015625);
    return max((PositivePow(param, param_1) * 1.05499994754791259765625) - vec3(0.054999999701976776123046875), vec3(0.0));
}

highp vec4 LinearToSRGB(highp vec4 c)
{
    highp vec3 param = c.xyz;
    return vec4(LinearToSRGB(param), c.w);
}

highp vec4 ConvertFromSRGBTexture(highp vec4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    highp vec4 param = c;
    return LinearToSRGB(param);
}

highp vec3 SRGBToLinear(highp vec3 c)
{
    return min(c, c * ((c * ((c * 0.305306017398834228515625) + vec3(0.6821711063385009765625))) + vec3(0.01252287812530994415283203125)));
}

highp vec4 SRGBToLinear(highp vec4 c)
{
    highp vec3 param = c.xyz;
    return vec4(SRGBToLinear(param), c.w);
}

highp vec4 ConvertToScreen(highp vec4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    highp vec4 param = c;
    return SRGBToLinear(param);
}

highp vec4 _main(PS_Input Input)
{
    bool convertColorSpace = !(CBPS0.miscFlags.x == 0.0);
    highp vec4 param = texture2D(Sampler_sampler_colorTex, Input.UV);
    bool param_1 = convertColorSpace;
    highp vec4 Output = ConvertFromSRGBTexture(param, param_1) * Input.Color;
    highp vec3 texNormal = (texture2D(Sampler_sampler_normalTex, Input.UV).xyz - vec3(0.5)) * 2.0;
    highp vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    highp float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    highp vec3 _229 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + CBPS0.fLightAmbient.xyz);
    Output = vec4(_229.x, _229.y, _229.z, Output.w);
    highp vec3 _237 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_237.x, _237.y, _237.z, Output.w);
    if (Output.w == 0.0)
    {
        discard;
    }
    highp vec4 param_2 = Output;
    bool param_3 = convertColorSpace;
    return ConvertToScreen(param_2, param_3);
}

void main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    Input.WorldN = _VSPS_WorldN;
    Input.WorldB = _VSPS_WorldB;
    Input.WorldT = _VSPS_WorldT;
    Input.PosP = _VSPS_PosP;
    highp vec4 _284 = _main(Input);
    gl_FragData[0] = _284;
}

