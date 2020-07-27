#version 420

struct PS_Input
{
    vec4 Position;
    vec4 VColor;
    vec2 UV1;
    vec2 UV2;
    vec3 WorldP;
    vec3 WorldN;
    vec3 WorldT;
    vec3 WorldB;
    vec2 ScreenUV;
};

layout(set = 1, binding = 0, std140) uniform VS_ConstantBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
} _69;

layout(set = 1, binding = 2) uniform sampler2D Sampler_g_normalSampler;
layout(set = 1, binding = 1) uniform sampler2D Sampler_g_colorSampler;

layout(location = 0) in vec4 Input_VColor;
layout(location = 1) in vec2 Input_UV1;
layout(location = 2) in vec2 Input_UV2;
layout(location = 3) in vec3 Input_WorldP;
layout(location = 4) in vec3 Input_WorldN;
layout(location = 5) in vec3 Input_WorldT;
layout(location = 6) in vec3 Input_WorldB;
layout(location = 7) in vec2 Input_ScreenUV;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec3 loN = texture(Sampler_g_normalSampler, Input.UV1).xyz;
    vec3 texNormal = (loN - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    float diffuse = max(dot(_69.fLightDirection.xyz, localNormal), 0.0);
    vec4 Output = texture(Sampler_g_colorSampler, Input.UV1) * Input.VColor;
    vec3 _104 = Output.xyz * ((_69.fLightColor.xyz * diffuse) + vec3(_69.fLightAmbient.xyz));
    Output = vec4(_104.x, _104.y, _104.z, Output.w);
    if (Output.w == 0.0)
    {
        discard;
    }
    return Output;
}

void main()
{
    PS_Input Input;
    Input.Position = gl_FragCoord;
    Input.VColor = Input_VColor;
    Input.UV1 = Input_UV1;
    Input.UV2 = Input_UV2;
    Input.WorldP = Input_WorldP;
    Input.WorldN = Input_WorldN;
    Input.WorldT = Input_WorldT;
    Input.WorldB = Input_WorldB;
    Input.ScreenUV = Input_ScreenUV;
    vec4 _158 = _main(Input);
    _entryPointOutput = _158;
}

