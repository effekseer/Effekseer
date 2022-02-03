static const char gl_postfx_extract_ps[] = R"(
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
    vec4 g_filterParams;
    vec4 g_intensity;
};

uniform _Global CBPS0;

uniform sampler2D Sampler_g_sampler;

in vec2 _VSPS_UV;
layout(location = 0) out vec4 _entryPointOutput;

bool isNan(float val)
{
    return (((val < 0.0) || (0.0 < val)) || (val == 0.0)) ? false : true;
}

vec4 _main(PS_Input Input)
{
    vec3 color = texture(Sampler_g_sampler, Input.UV).xyz;
    float param = color.x;
    if (isNan(param))
    {
        color.x = 0.0;
    }
    float param_1 = color.y;
    if (isNan(param_1))
    {
        color.y = 0.0;
    }
    float param_2 = color.z;
    if (isNan(param_2))
    {
        color.z = 0.0;
    }
    float brightness = dot(color, vec3(0.2989999949932098388671875, 0.58700001239776611328125, 0.114000000059604644775390625));
    float soft = brightness - CBPS0.g_filterParams.y;
    soft = clamp(soft, 0.0, CBPS0.g_filterParams.z);
    soft = (soft * soft) * CBPS0.g_filterParams.w;
    float contribution = max(soft, brightness - CBPS0.g_filterParams.x);
    contribution /= max(brightness, 9.9999997473787516355514526367188e-06);
    return vec4((color * contribution) * CBPS0.g_intensity.x, 1.0);
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.UV = _VSPS_UV;
    _entryPointOutput = _main(Input);
}

)";

