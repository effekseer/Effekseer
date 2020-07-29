#version 420

struct PS_Input
{
    vec4 Position;
    vec4 Color;
    vec2 UV;
    vec4 Pos;
    vec4 PosU;
    vec4 PosR;
};

layout(set = 1, binding = 0, std140) uniform VS_ConstantBuffer
{
    vec4 g_scale;
    vec4 mUVInversedBack;
} _73;

layout(set = 1, binding = 1) uniform sampler2D Sampler_g_sampler;
layout(set = 1, binding = 2) uniform sampler2D Sampler_g_backSampler;

layout(location = 0) in vec4 Input_Color;
layout(location = 1) in vec2 Input_UV;
layout(location = 2) in vec4 Input_Pos;
layout(location = 3) in vec4 Input_PosU;
layout(location = 4) in vec4 Input_PosR;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec4 Output = texture(Sampler_g_sampler, Input.UV);
    Output.w *= Input.Color.w;
    vec2 pos = Input.Pos.xy / vec2(Input.Pos.w);
    vec2 posU = Input.PosU.xy / vec2(Input.PosU.w);
    vec2 posR = Input.PosR.xy / vec2(Input.PosR.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * _73.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * _73.g_scale.x;
    vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = _73.mUVInversedBack.x + (_73.mUVInversedBack.y * uv.y);
    vec3 color = vec3(texture(Sampler_g_backSampler, uv).xyz);
    Output = vec4(color.x, color.y, color.z, Output.w);
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
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.Pos = Input_Pos;
    Input.PosU = Input_PosU;
    Input.PosR = Input_PosR;
    vec4 _178 = _main(Input);
    _entryPointOutput = _178;
}

