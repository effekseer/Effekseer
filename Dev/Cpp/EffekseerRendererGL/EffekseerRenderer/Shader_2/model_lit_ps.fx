#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 PosVS;
    vec2 UV;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
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

uniform sampler2D Sampler_g_colorSampler;
uniform sampler2D Sampler_g_normalSampler;

centroid varying vec2 _VSPS_UV;
varying vec3 _VSPS_Normal;
varying vec3 _VSPS_Binormal;
varying vec3 _VSPS_Tangent;
centroid varying vec4 _VSPS_Color;
varying vec4 _VSPS_PosP;

vec4 _main(PS_Input Input)
{
    vec4 Output = texture2D(Sampler_g_colorSampler, Input.UV) * Input.Color;
    vec3 texNormal = (texture2D(Sampler_g_normalSampler, Input.UV).xyz - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.Tangent), vec3(Input.Binormal), vec3(Input.Normal)) * texNormal);
    float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    vec3 _99 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + CBPS0.fLightAmbient.xyz);
    Output = vec4(_99.x, _99.y, _99.z, Output.w);
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
    vec4 _145 = _main(Input);
    gl_FragData[0] = _145;
}

