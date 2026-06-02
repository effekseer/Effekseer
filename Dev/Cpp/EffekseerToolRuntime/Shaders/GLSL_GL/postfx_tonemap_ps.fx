#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec2 UV;
};

struct PS_ConstantBuffer
{
    vec4 g_toneparams;
};

uniform PS_ConstantBuffer CBPS0;

layout(binding = 0) uniform sampler2D Sampler_g_sampler;

in vec2 _VSPS_UV;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec3 texel = texture(Sampler_g_sampler, Input.UV).xyz;
    float lum = CBPS0.g_toneparams.x * dot(texel, vec3(0.2989999949932098388671875, 0.58700001239776611328125, 0.114000000059604644775390625));
    lum /= (1.0 + lum);
    return vec4(texel * lum, 1.0);
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.UV = _VSPS_UV;
    _entryPointOutput = _main(Input);
}

