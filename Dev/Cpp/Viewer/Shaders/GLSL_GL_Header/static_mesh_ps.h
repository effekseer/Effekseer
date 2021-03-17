static const char gl_static_mesh_ps[] = R"(
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
    vec4 Position;
};

struct PS_OUTPUT
{
    vec4 o0;
    vec4 o1;
};

struct PS_ConstantBuffer
{
    vec4 isLit;
    vec4 directionalLightDirection;
    vec4 directionalLightColor;
    vec4 ambientLightColor;
};

uniform PS_ConstantBuffer CBPS0;

uniform sampler2D Sampler_g_sampler;

in vec2 _VSPS_UV;
in vec4 _VSPS_Color;
in vec3 _VSPS_Normal;
in vec4 _VSPS_Position;
layout(location = 0) out vec4 _entryPointOutput_o0;
layout(location = 1) out vec4 _entryPointOutput_o1;

PS_OUTPUT _main(PS_Input Input)
{
    PS_OUTPUT _output;
    _output.o0 = Input.Color * texture(Sampler_g_sampler, Input.UV);
    if (CBPS0.isLit.x > 0.0)
    {
        float diffuse = max(dot(CBPS0.directionalLightDirection.xyz, Input.Normal), 0.0);
        vec3 _73 = _output.o0.xyz * ((CBPS0.directionalLightColor.xyz * diffuse) + CBPS0.ambientLightColor.xyz);
        _output.o0 = vec4(_73.x, _73.y, _73.z, _output.o0.w);
    }
    _output.o1 = vec4(1.0);
    _output.o1.x = Input.Position.z / Input.Position.w;
    return _output;
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.UV = _VSPS_UV;
    Input.Color = _VSPS_Color;
    Input.Normal = _VSPS_Normal;
    Input.Position = _VSPS_Position;
    PS_OUTPUT flattenTemp = _main(Input);
    _entryPointOutput_o0 = flattenTemp.o0;
    _entryPointOutput_o1 = flattenTemp.o1;
}

)";

