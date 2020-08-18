#version 420

struct PS_Input
{
    vec4 Pos;
    vec2 UV;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec4 Color;
};

layout(set = 1, binding = 0, std140) uniform VS_ConstantBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
} _79;

layout(set = 1, binding = 1) uniform sampler2D Sampler_g_colorSampler;
layout(set = 1, binding = 2) uniform sampler2D Sampler_g_normalSampler;

layout(location = 0) in vec2 Input_UV;
layout(location = 1) in vec3 Input_Normal;
layout(location = 2) in vec3 Input_Binormal;
layout(location = 3) in vec3 Input_Tangent;
layout(location = 4) in vec4 Input_Color;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec4 Output = texture(Sampler_g_colorSampler, Input.UV) * Input.Color;
    vec3 texNormal = (texture(Sampler_g_normalSampler, Input.UV).xyz - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.Tangent), vec3(Input.Binormal), vec3(Input.Normal)) * texNormal);
    float diffuse = max(dot(_79.fLightDirection.xyz, localNormal), 0.0);
    vec3 _99 = Output.xyz * ((_79.fLightColor.xyz * diffuse) + _79.fLightAmbient.xyz);
    Output = vec4(_99.x, _99.y, _99.z, Output.w);
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
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.Color = Input_Color;
    vec4 _141 = _main(Input);
    _entryPointOutput = _141;
}

