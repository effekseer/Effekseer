static const char gl_postfx_overdraw_ps[] = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec2 UV;
};

uniform sampler2D Sampler_g_sampler;

in vec2 _VSPS_UV;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    float value = texture(Sampler_g_sampler, Input.UV).x / 8.0;
    return vec4(value, 1.0 - value, 0.0, 1.0);
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.UV = _VSPS_UV;
    _entryPointOutput = _main(Input);
}

)";

