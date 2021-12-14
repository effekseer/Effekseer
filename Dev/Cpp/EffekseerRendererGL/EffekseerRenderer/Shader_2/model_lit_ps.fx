#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 PosVS;
    vec4 Color;
    vec2 UV;
    vec3 WorldN;
    vec3 WorldB;
    vec3 WorldT;
    vec4 PosP;
};

struct PS_ConstanBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
    vec4 fCameraFrontDirection;
    vec4 fFalloffParameter;
    vec4 fFalloffBeginColor;
    vec4 fFalloffEndColor;
    vec4 fEmissiveScaling;
    vec4 fEdgeColor;
    vec4 fEdgeParameter;
    vec4 softParticleParam;
    vec4 reconstructionParam1;
    vec4 reconstructionParam2;
    vec4 mUVInversedBack;
    vec4 miscFlags;
};

uniform PS_ConstanBuffer CBPS0;

uniform sampler2D Sampler_sampler_colorTex;
uniform sampler2D Sampler_sampler_normalTex;

centroid varying vec4 _VSPS_Color;
centroid varying vec2 _VSPS_UV;
varying vec3 _VSPS_WorldN;
varying vec3 _VSPS_WorldB;
varying vec3 _VSPS_WorldT;
varying vec4 _VSPS_PosP;

vec3 PositivePow(vec3 base, vec3 power)
{
    return pow(max(abs(base), vec3(1.1920928955078125e-07)), power);
}

vec3 LinearToSRGB(vec3 c)
{
    vec3 param = c;
    vec3 param_1 = vec3(0.4166666567325592041015625);
    return max((PositivePow(param, param_1) * 1.05499994754791259765625) - vec3(0.054999999701976776123046875), vec3(0.0));
}

vec4 LinearToSRGB(vec4 c)
{
    vec3 param = c.xyz;
    return vec4(LinearToSRGB(param), c.w);
}

vec4 ConvertFromSRGBTexture(vec4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    vec4 param = c;
    return LinearToSRGB(param);
}

vec3 SRGBToLinear(vec3 c)
{
    return min(c, c * ((c * ((c * 0.305306017398834228515625) + vec3(0.6821711063385009765625))) + vec3(0.01252287812530994415283203125)));
}

vec4 SRGBToLinear(vec4 c)
{
    vec3 param = c.xyz;
    return vec4(SRGBToLinear(param), c.w);
}

vec4 ConvertToScreen(vec4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    vec4 param = c;
    return SRGBToLinear(param);
}

vec4 _main(PS_Input Input)
{
    bool convertColorSpace = !(CBPS0.miscFlags.x == 0.0);
    vec4 param = texture2D(Sampler_sampler_colorTex, Input.UV);
    bool param_1 = convertColorSpace;
    vec4 Output = ConvertFromSRGBTexture(param, param_1) * Input.Color;
    vec3 texNormal = (texture2D(Sampler_sampler_normalTex, Input.UV).xyz - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    vec3 _229 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + CBPS0.fLightAmbient.xyz);
    Output = vec4(_229.x, _229.y, _229.z, Output.w);
    vec3 _237 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_237.x, _237.y, _237.z, Output.w);
    if (Output.w == 0.0)
    {
        discard;
    }
    vec4 param_2 = Output;
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
    vec4 _284 = _main(Input);
    gl_FragData[0] = _284;
}

