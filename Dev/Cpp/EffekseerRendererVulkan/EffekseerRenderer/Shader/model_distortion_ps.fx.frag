#version 420

struct PS_Input
{
    vec4 Position;
    vec2 UV;
    vec4 Normal;
    vec4 Binormal;
    vec4 Tangent;
    vec4 Pos;
    vec4 Color;
};

layout(set = 1, binding = 0, std140) uniform PS_ConstanBuffer
{
    vec4 g_scale;
    vec4 mUVInversedBack;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
} _73;

layout(set = 1, binding = 1) uniform sampler2D Sampler_g_sampler;
layout(set = 1, binding = 2) uniform sampler2D Sampler_g_backSampler;

layout(location = 0) in vec2 Input_UV;
layout(location = 1) in vec4 Input_Normal;
layout(location = 2) in vec4 Input_Binormal;
layout(location = 3) in vec4 Input_Tangent;
layout(location = 4) in vec4 Input_Pos;
layout(location = 5) in vec4 Input_Color;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec4 Output = texture(Sampler_g_sampler, Input.UV);
    Output.w *= Input.Color.w;
    vec2 pos = Input.Pos.xy / vec2(Input.Pos.w);
    vec2 posU = Input.Tangent.xy / vec2(Input.Tangent.w);
    vec2 posR = Input.Binormal.xy / vec2(Input.Binormal.w);
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
    Input.UV = Input_UV;
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    vec4 _182 = _main(Input);
    _entryPointOutput = _182;
}

