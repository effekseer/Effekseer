#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec2 UV;
};

layout(binding = 0) uniform sampler2D Sampler_g_sampler;
layout(binding = 0) uniform sampler2D Sampler_g_sampler_1;
layout(binding = 0) uniform sampler2D Sampler_g_sampler_2;
layout(binding = 0) uniform sampler2D Sampler_g_sampler_3;

in vec2 _VSPS_UV;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec3 c0 = texture(Sampler_g_sampler, Input.UV).xyz;
    vec3 c1 = texture(Sampler_g_sampler_1, Input.UV).xyz;
    vec3 c2 = texture(Sampler_g_sampler_2, Input.UV).xyz;
    vec3 c3 = texture(Sampler_g_sampler_3, Input.UV).xyz;
    return vec4(((c0 + c1) + c2) + c3, 1.0);
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.UV = _VSPS_UV;
    _entryPointOutput = _main(Input);
}

