static const char gl_postfx_downsample_ps[] = R"(
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

uniform sampler2D Sampler_g_sampler;

in vec2 _VSPS_UV;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec2 size = CBPS0.g_size.xy;
    vec4 color1 = texture(Sampler_g_sampler, Input.UV + (vec2(-0.5) / size));
    vec4 color2 = texture(Sampler_g_sampler, Input.UV + (vec2(0.5, -0.5) / size));
    vec4 color3 = texture(Sampler_g_sampler, Input.UV + (vec2(-0.5, 0.5) / size));
    vec4 color4 = texture(Sampler_g_sampler, Input.UV + (vec2(0.5) / size));
    return (((color1 + color2) + color3) + color4) * 0.25;
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.UV = _VSPS_UV;
    _entryPointOutput = _main(Input);
}

)";

