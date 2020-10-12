#version 420

struct PS_Input
{
    vec4 Pos;
    vec2 UV;
    vec4 Color;
};

layout(set = 1, binding = 0, std140) uniform VS_ConstantBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
} _68;

layout(set = 1, binding = 1) uniform sampler2D Sampler_g_colorSampler;

layout(location = 0) centroid in vec2 Input_UV;
layout(location = 1) centroid in vec4 Input_Color;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec4 Output = texture(Sampler_g_colorSampler, Input.UV) * Input.Color;
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
    Input.UV = Input_UV;
    Input.Color = Input_Color;
    vec4 _65 = _main(Input);
    _entryPointOutput = _65;
}

