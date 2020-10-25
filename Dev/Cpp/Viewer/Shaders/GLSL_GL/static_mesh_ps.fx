#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec2 UV;
    vec4 Color;
    vec3 Normal;
};

struct PS_ConstantBuffer
{
    vec4 directionalLightDirection;
    vec4 directionalLightColor;
    vec4 ambientLightColor;
};

uniform PS_ConstantBuffer CBPS0;

in vec2 _VSPS_UV;
in vec4 _VSPS_Color;
in vec3 _VSPS_Normal;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec4 _output = Input.Color;
    float diffuse = max(dot(CBPS0.directionalLightDirection.xyz, Input.Normal), 0.0);
    vec3 _47 = _output.xyz * ((CBPS0.directionalLightColor.xyz * diffuse) + CBPS0.ambientLightColor.xyz);
    _output = vec4(_47.x, _47.y, _47.z, _output.w);
    if (_output.w == 0.0)
    {
        discard;
    }
    return _output;
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.UV = _VSPS_UV;
    Input.Color = _VSPS_Color;
    Input.Normal = _VSPS_Normal;
    vec4 _84 = _main(Input);
    _entryPointOutput = _84;
}

