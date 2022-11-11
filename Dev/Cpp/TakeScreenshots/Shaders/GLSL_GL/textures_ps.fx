#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec2 UV;
    vec4 Color;
};

layout(binding = 0) uniform sampler2D Sampler_g_sampler1;
layout(binding = 1) uniform sampler2DArray Sampler_g_sampler2;
layout(binding = 2) uniform sampler3D Sampler_g_sampler3;

in vec2 _VSPS_UV;
in vec4 _VSPS_Color;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    if (Input.UV.x < 0.300000011920928955078125)
    {
        return texture(Sampler_g_sampler1, Input.UV);
    }
    else
    {
        if (Input.UV.x < 0.60000002384185791015625)
        {
            return texture(Sampler_g_sampler2, vec3(Input.UV, 1.0));
        }
    }
    return texture(Sampler_g_sampler3, vec3(Input.UV, 0.5));
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.UV = _VSPS_UV;
    Input.Color = _VSPS_Color;
    _entryPointOutput = _main(Input);
}

