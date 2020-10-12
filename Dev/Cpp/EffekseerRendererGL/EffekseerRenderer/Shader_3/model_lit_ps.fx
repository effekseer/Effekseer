#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec2 UV;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec4 Color;
};

struct VS_ConstantBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
};

uniform VS_ConstantBuffer CBPS0;

layout(binding = 0) uniform sampler2D Sampler_g_colorSampler;
layout(binding = 1) uniform sampler2D Sampler_g_normalSampler;

centroid in vec2 _VSPS_UV;
in vec3 _VSPS_Normal;
in vec3 _VSPS_Binormal;
in vec3 _VSPS_Tangent;
centroid in vec4 _VSPS_Color;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec4 Output = texture(Sampler_g_colorSampler, Input.UV) * Input.Color;
    vec3 texNormal = (texture(Sampler_g_normalSampler, Input.UV).xyz - vec3(0.5)) * 2.0;
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
    Input.Pos = gl_FragCoord;
    Input.UV = _VSPS_UV;
    Input.Normal = _VSPS_Normal;
    Input.Binormal = _VSPS_Binormal;
    Input.Tangent = _VSPS_Tangent;
    Input.Color = _VSPS_Color;
    vec4 _141 = _main(Input);
    _entryPointOutput = _141;
}

