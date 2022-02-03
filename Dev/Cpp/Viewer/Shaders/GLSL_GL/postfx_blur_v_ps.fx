#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec2 UV;
};

struct _Global
{
    vec4 g_size;
};

uniform _Global CBPS0;

layout(binding = 0) uniform sampler2D Sampler_g_sampler;

in vec2 _VSPS_UV;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec2 size = CBPS0.g_size.xy;
    vec4 color = texture(Sampler_g_sampler, Input.UV) * 0.2230674326419830322265625;
    color += (texture(Sampler_g_sampler, Input.UV + vec2(0.0, (-5.152032375335693359375) / size.y)) * 0.0052916849963366985321044921875);
    color += (texture(Sampler_g_sampler, Input.UV + vec2(0.0, (-3.2509129047393798828125) / size.y)) * 0.07297551631927490234375);
    color += (texture(Sampler_g_sampler, Input.UV + vec2(0.0, (-1.38491213321685791015625) / size.y)) * 0.310199081897735595703125);
    color += (texture(Sampler_g_sampler, Input.UV + vec2(0.0, 1.38491213321685791015625 / size.y)) * 0.310199081897735595703125);
    color += (texture(Sampler_g_sampler, Input.UV + vec2(0.0, 3.2509129047393798828125 / size.y)) * 0.07297551631927490234375);
    color += (texture(Sampler_g_sampler, Input.UV + vec2(0.0, 5.152032375335693359375 / size.y)) * 0.0052916849963366985321044921875);
    return vec4(color.xyz, 1.0);
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.UV = _VSPS_UV;
    _entryPointOutput = _main(Input);
}

